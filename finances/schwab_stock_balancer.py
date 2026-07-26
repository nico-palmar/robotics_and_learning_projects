#!/usr/bin/env python3
"""Recommend category-level purchases from a Schwab Positions CSV export.

This tool is intentionally local and advisory-only: it never selects a fund,
connects to Schwab, or places a trade.
"""

from __future__ import annotations

import argparse
import csv
import json
import sys
from dataclasses import dataclass
from decimal import Decimal, InvalidOperation, ROUND_HALF_UP
from pathlib import Path
from typing import Iterable


MONEY = Decimal("0.01")
ONE_HUNDRED = Decimal("100")


class BalancerError(ValueError):
	"""Raised when an export or allocation configuration cannot be used."""


@dataclass(frozen=True)
class Category:
	key: str
	label: str
	target: Decimal


@dataclass(frozen=True)
class Position:
	symbol: str
	market_value: Decimal


@dataclass
class Analysis:
	categories: dict[str, Category]
	symbol_mapping: dict[str, str]
	current: dict[str, Decimal]
	purchases: dict[str, Decimal]
	projected: dict[str, Decimal]
	total_value: Decimal
	investable_cash: Decimal
	unallocated_cash: Decimal
	alerts: list[str]


def parse_money(value: str) -> Decimal:
	"""Parse a Schwab currency field into a non-empty Decimal."""
	cleaned = value.strip().replace("$", "").replace(",", "")
	if not cleaned or cleaned in {"--", "N/A"}:
		raise BalancerError("missing market value")
	if cleaned.startswith("(") and cleaned.endswith(")"):
		cleaned = f"-{cleaned[1:-1]}"
	try:
		return Decimal(cleaned)
	except InvalidOperation as error:
		raise BalancerError(f"invalid market value {value!r}") from error


def load_positions(csv_path: Path) -> list[Position]:
	"""Read positions and cash balances from a Schwab export, ignoring totals."""
	try:
		with csv_path.open(newline="", encoding="utf-8-sig") as file:
			rows = list(csv.reader(file))
	except OSError as error:
		raise BalancerError(f"could not read CSV: {error}") from error

	header_index = next(
		(index for index, row in enumerate(rows) if row and row[0].strip() == "Symbol"),
		None,
	)
	if header_index is None:
		raise BalancerError("could not find the Schwab 'Symbol' header row")

	header = rows[header_index]
	required_columns = {"Symbol", "Mkt Val (Market Value)"}
	if not required_columns.issubset(header):
		raise BalancerError("CSV is missing 'Symbol' or 'Mkt Val (Market Value)'")

	positions: list[Position] = []
	summary_symbols = {"POSITIONS TOTAL"}
	for values in rows[header_index + 1 :]:
		row = dict(zip(header, values))
		symbol = (row.get("Symbol") or "").strip().upper()
		if not symbol or symbol in summary_symbols or symbol == "--":
			continue
		try:
			market_value = parse_money(row.get("Mkt Val (Market Value)") or "")
		except BalancerError as error:
			raise BalancerError(f"{symbol}: {error}") from error
		positions.append(Position(symbol=symbol, market_value=market_value))

	if not positions:
		raise BalancerError("CSV contains no usable position market values")
	return positions


def load_mapping(mapping_path: Path) -> tuple[dict[str, Category], dict[str, str]]:
	"""Load and validate configured categories and symbol mappings."""
	try:
		with mapping_path.open(encoding="utf-8") as file:
			data = json.load(file)
	except (OSError, json.JSONDecodeError) as error:
		raise BalancerError(f"could not load mapping file: {error}") from error

	raw_categories = data.get("categories")
	raw_symbols = data.get("symbols")
	if not isinstance(raw_categories, dict) or not isinstance(raw_symbols, dict):
		raise BalancerError("mapping must contain 'categories' and 'symbols' objects")

	categories: dict[str, Category] = {}
	try:
		for key, item in raw_categories.items():
			categories[key] = Category(key, item["label"], Decimal(str(item["target"])))
	except (KeyError, TypeError, InvalidOperation) as error:
		raise BalancerError("each category needs a label and decimal target") from error

	if not categories or sum(category.target for category in categories.values()) != Decimal("1"):
		raise BalancerError("category targets must total exactly 1.0")
	if "cash" not in categories:
		raise BalancerError("mapping must include a 'cash' category")

	symbols = {str(symbol).upper(): str(category) for symbol, category in raw_symbols.items()}
	invalid_categories = sorted(set(symbols.values()) - set(categories))
	if invalid_categories:
		raise BalancerError(f"symbols map to unknown categories: {', '.join(invalid_categories)}")
	return categories, symbols


def save_mapping(mapping_path: Path, categories: dict[str, Category], symbols: dict[str, str]) -> None:
	"""Persist a newly classified symbol without changing category targets."""
	data = {
		"categories": {
			key: {"label": category.label, "target": str(category.target)}
			for key, category in categories.items()
		},
		"symbols": dict(sorted(symbols.items())),
	}
	mapping_path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def classify_unknown_symbols(
	symbols: Iterable[str], categories: dict[str, Category], known_symbols: dict[str, str], mapping_path: Path
) -> None:
	"""Prompt once for every unknown symbol and save valid choices."""
	unknown = sorted(set(symbols) - set(known_symbols))
	if not unknown:
		return

	choices = ", ".join(f"{key} ({category.label})" for key, category in categories.items())
	print("New symbols need a category before this portfolio can be analyzed.")
	for symbol in unknown:
		while True:
			answer = input(f"Category for {symbol} [{choices}]: ").strip()
			if answer in categories:
				known_symbols[symbol] = answer
				break
			print("Please enter one of the category keys shown in brackets.")
	save_mapping(mapping_path, categories, known_symbols)
	print(f"Saved {len(unknown)} new symbol mapping(s) to {mapping_path}.")


def round_money(value: Decimal) -> Decimal:
	return value.quantize(MONEY, rounding=ROUND_HALF_UP)


def analyze_portfolio(
	positions: Iterable[Position], categories: dict[str, Category], symbol_mapping: dict[str, str]
) -> Analysis:
	"""Calculate shortfall-only purchases and post-purchase drift warnings."""
	current = {key: Decimal("0") for key in categories}
	for position in positions:
		try:
			current[symbol_mapping[position.symbol]] += position.market_value
		except KeyError as error:
			raise BalancerError(f"no category mapping for {position.symbol}") from error

	total_value = sum(current.values())
	if total_value <= 0:
		raise BalancerError("portfolio market value must be greater than zero")

	cash_reserve = categories["cash"].target * total_value
	investable_cash = max(Decimal("0"), current["cash"] - cash_reserve)
	shortfalls = {
		key: max(Decimal("0"), category.target * total_value - current[key])
		for key, category in categories.items()
		if key != "cash"
	}
	purchase_total = min(investable_cash, sum(shortfalls.values()))
	purchases = {key: Decimal("0") for key in categories}
	if purchase_total:
		total_shortfall = sum(shortfalls.values())
		allocated = Decimal("0")
		active_keys = [key for key, shortfall in shortfalls.items() if shortfall]
		for key in active_keys[:-1]:
			amount = round_money(purchase_total * shortfalls[key] / total_shortfall)
			purchases[key] = amount
			allocated += amount
		purchases[active_keys[-1]] = round_money(purchase_total - allocated)

	actual_purchase_total = sum(purchases.values())
	projected = dict(current)
	for key, amount in purchases.items():
		projected[key] += amount
	projected["cash"] -= actual_purchase_total
	unallocated_cash = round_money(investable_cash - actual_purchase_total)

	alerts: list[str] = []
	for key, category in categories.items():
		actual_percent = projected[key] / total_value
		# TODO: Re-evaluate this hybrid tolerance against lived portfolio behavior.
		tolerance = Decimal("0.01") + Decimal("0.10") * category.target
		deviation = actual_percent - category.target
		if abs(deviation) > tolerance:
			direction = "above" if deviation > 0 else "below"
			alerts.append(
				f"{category.label} is {abs(deviation) * ONE_HUNDRED:.2f} percentage points {direction} target "
				f"(tolerance: {tolerance * ONE_HUNDRED:.2f})."
			)

	return Analysis(
		categories=categories,
		symbol_mapping=dict(symbol_mapping),
		current=current,
		purchases=purchases,
		projected=projected,
		total_value=total_value,
		investable_cash=round_money(investable_cash),
		unallocated_cash=unallocated_cash,
		alerts=alerts,
	)


def format_currency(value: Decimal) -> str:
	return f"${round_money(value):,.2f}"


def print_analysis(analysis: Analysis) -> None:
	"""Print current/projected allocations and category-level buy recommendations."""
	print(f"Portfolio market value: {format_currency(analysis.total_value)}\n")
	print(f"{'Category':<30} {'Current':>14} {'Current %':>11} {'After buy %':>13} {'Target':>9}")
	print("-" * 84)
	for key, category in analysis.categories.items():
		current_percent = analysis.current[key] / analysis.total_value * ONE_HUNDRED
		projected_percent = analysis.projected[key] / analysis.total_value * ONE_HUNDRED
		print(
			f"{category.label:<30} {format_currency(analysis.current[key]):>14} "
			f"{current_percent:>10.2f}% {projected_percent:>12.2f}% {category.target * ONE_HUNDRED:>8.2f}%"
		)

	print(f"\nCash above the 5% reserve: {format_currency(analysis.investable_cash)}")
	purchases = [(key, amount) for key, amount in analysis.purchases.items() if amount > 0]
	if purchases:
		purchase_total = sum(amount for _, amount in purchases)
		print("Recommended purchases (percent of cash to invest):")
		for key, amount in purchases:
			print(
				f"  {analysis.categories[key].label}: {format_currency(amount)} "
				f"({amount / purchase_total * ONE_HUNDRED:.1f}% of cash to invest)"
			)
	else:
		print("No category purchases are recommended.")
	if analysis.unallocated_cash > 0:
		print(f"Cash left unallocated: {format_currency(analysis.unallocated_cash)} (buying would worsen allocation).")

	if analysis.alerts:
		print("\nWARNING: material drift remains after the proposed purchases:")
		for alert in analysis.alerts:
			print(f"  - {alert}")
	else:
		print("\nNo category remains outside the configured drift tolerance.")

	print("\nConfigured category mappings:")
	print(f"{'Category':<30} {'Target':>8}  Symbols")
	print("-" * 84)
	for key, category in analysis.categories.items():
		symbols = sorted(
			symbol for symbol, mapped_category in analysis.symbol_mapping.items() if mapped_category == key
		)
		mapped_symbols = ", ".join(symbols) if symbols else "No symbols mapped"
		print(f"{category.label:<30} {category.target * ONE_HUNDRED:>7.2f}%  {mapped_symbols}")


def main(argv: list[str] | None = None) -> int:
	parser = argparse.ArgumentParser(description="Recommend category-level purchases from a Schwab CSV export.")
	parser.add_argument("csv_path", type=Path, help="Path to a Schwab Individual Positions CSV export")
	parser.add_argument(
		"--mapping",
		type=Path,
		default=Path(__file__).with_name("category_mappings.json"),
		help="Path to category mapping JSON (default: sibling category_mappings.json)",
	)
	args = parser.parse_args(argv)

	try:
		categories, symbol_mapping = load_mapping(args.mapping)
		positions = load_positions(args.csv_path)
		classify_unknown_symbols((position.symbol for position in positions), categories, symbol_mapping, args.mapping)
		print_analysis(analyze_portfolio(positions, categories, symbol_mapping))
	except (BalancerError, EOFError) as error:
		print(f"Error: {error}", file=sys.stderr)
		return 2
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
