from __future__ import annotations

import sys
import tempfile
import unittest
from collections.abc import Mapping
from contextlib import redirect_stdout
from decimal import Decimal
from io import StringIO
from pathlib import Path
from typing import ClassVar

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from schwab_stock_balancer import (  # noqa: E402
    Analysis,
    BalancerError,
    Category,
    Position,
    analyze_portfolio,
    load_mapping,
    load_positions,
    parse_money,
    print_analysis,
)


ROOT: Path = Path(__file__).resolve().parents[1]
MAPPING: Path = ROOT / "category_mappings.json"

# These aliases keep allocation scenarios readable. The implementation receives
# tickers, but a test should describe the investor's category-level intent.
US_BROAD_MARKET: str = "us_broad_market"
EMERGING_MARKETS: str = "emerging_markets"
INTERNATIONAL_MARKETS: str = "international_markets"
REAL_ESTATE: str = "real_estate"
BONDS: str = "bonds"
CASH: str = "cash"

# Use a round portfolio total so the target dollars are visible in every test:
# $1,000 x 35% = $350 U.S. broad market, for example.
PORTFOLIO_TOTAL: Decimal = Decimal("1000")


def positions(values: Mapping[str, Decimal | str | int | float]) -> list[Position]:
    return [Position(symbol, Decimal(str(value))) for symbol, value in values.items()]


def portfolio_from_category_amounts(
    category_amounts: Mapping[str, Decimal | str | int | float],
    symbols_by_category: Mapping[str, str],
) -> list[Position]:
    """Express a test portfolio by allocation category, not ticker symbols."""
    return positions({symbols_by_category[category]: amount for category, amount in category_amounts.items()})


def target_amounts_from_total(
    categories: Mapping[str, Category], total: Decimal = PORTFOLIO_TOTAL
) -> dict[str, Decimal]:
    """Convert configured target allocation percentages into category dollars."""
    return {key: total * category.target for key, category in categories.items()}


class MoneyParsingTests(unittest.TestCase):
    def test_currency_and_parentheses(self) -> None:
        """Parse formatted positive and parenthesized negative currency values."""
        self.assertEqual(parse_money("$1,234.50"), Decimal("1234.50"))
        self.assertEqual(parse_money("($25.10)"), Decimal("-25.10"))

    def test_missing_value_is_rejected(self) -> None:
        """Reject export placeholders that do not contain a market value."""
        with self.assertRaisesRegex(BalancerError, "missing market value"):
            parse_money("--")


class CsvParsingTests(unittest.TestCase):
    def test_detects_header_preserves_cash_and_ignores_total(self) -> None:
        """Find the header, preserve the cash balance, and omit the portfolio total."""
        content: str = """Account metadata,,,,,,\n,,,,,,\nSymbol,Description,Mkt Val (Market Value)\nSWTSX,Fund,$350.00\nSWVXX,Cash,$50.00\nCash & Cash Investments,--,$50.00\nPositions Total,,$450.00\n"""
        with tempfile.TemporaryDirectory() as directory:
            path: Path = Path(directory) / "positions.csv"
            path.write_text(content, encoding="utf-8")
            self.assertEqual(
                load_positions(path),
                [
                    Position("SWTSX", Decimal("350.00")),
                    Position("SWVXX", Decimal("50.00")),
                    Position("CASH & CASH INVESTMENTS", Decimal("50.00")),
                ],
            )

    def test_blank_market_value_is_rejected(self) -> None:
        """Identify a symbol when its CSV market-value field is blank."""
        content: str = "Symbol,Mkt Val (Market Value)\nSWTSX,\n"
        with tempfile.TemporaryDirectory() as directory:
            path: Path = Path(directory) / "positions.csv"
            path.write_text(content, encoding="utf-8")
            with self.assertRaisesRegex(BalancerError, "SWTSX: missing"):
                load_positions(path)


class OutputTests(unittest.TestCase):
    def test_purchase_percentages_are_labeled_as_cash_allocations(self) -> None:
        """Label buy percentages as shares of the cash being invested, not the portfolio."""
        categories: dict[str, Category] = {
            "stocks": Category("stocks", "Stocks", Decimal("0.60")),
            "bonds": Category("bonds", "Bonds", Decimal("0.35")),
            CASH: Category(CASH, "Cash", Decimal("0.05")),
        }
        analysis: Analysis = Analysis(
            categories=categories,
            symbol_mapping={"BND": "bonds", "CASH": CASH, "STOCK": "stocks"},
            current={"stocks": Decimal("350"), "bonds": Decimal("100"), CASH: Decimal("550")},
            purchases={"stocks": Decimal("250"), "bonds": Decimal("250"), CASH: Decimal("0")},
            projected={"stocks": Decimal("600"), "bonds": Decimal("350"), CASH: Decimal("50")},
            total_value=PORTFOLIO_TOTAL,
            investable_cash=Decimal("500"),
            unallocated_cash=Decimal("0"),
            alerts=[],
        )
        output_stream: StringIO = StringIO()

        with redirect_stdout(output_stream):
            print_analysis(analysis)

        output: str = output_stream.getvalue()
        self.assertIn("After buy %", output)
        self.assertIn("Recommended purchases (percent of cash to invest):", output)
        self.assertIn("Stocks: $250.00 (50.0% of cash to invest)", output)
        self.assertIn("Configured category mappings:", output)
        self.assertIn("Stocks                           60.00%  STOCK", output)


class AllocationTests(unittest.TestCase):
    categories: ClassVar[dict[str, Category]]
    symbols: ClassVar[dict[str, str]]
    symbols_by_category: ClassVar[dict[str, str]]

    @classmethod
    def setUpClass(cls: type[AllocationTests]) -> None:
        cls.categories, cls.symbols = load_mapping(MAPPING)
        cls.symbols_by_category = {
            category: symbol for symbol, category in cls.symbols.items()
        }

    def analyze_category_amounts(self, category_amounts: Mapping[str, Decimal]) -> Analysis:
        return analyze_portfolio(
            portfolio_from_category_amounts(category_amounts, self.symbols_by_category),
            self.categories,
            self.symbols,
        )

    def target_amounts(self) -> dict[str, Decimal]:
        return target_amounts_from_total(self.categories)

    def assert_category_amounts(
        self,
        actual: Mapping[str, Decimal],
        expected: Mapping[str, Decimal | str | int | float],
    ) -> None:
        self.assertEqual(
            actual,
            {category: Decimal(str(amount)) for category, amount in expected.items()},
        )

    def assert_balancing_preserves_total(self, result: Analysis) -> None:
        self.assertEqual(sum(result.current.values()), PORTFOLIO_TOTAL)
        self.assertEqual(sum(result.projected.values()), PORTFOLIO_TOTAL)

    def test_balanced_portfolio_needs_no_purchase_or_alert(self) -> None:
        """Leave a portfolio at its configured targets unchanged and unflagged."""
        balanced_amounts: dict[str, Decimal] = self.target_amounts()
        result: Analysis = self.analyze_category_amounts(balanced_amounts)
        self.assert_category_amounts(result.current, balanced_amounts)
        self.assert_category_amounts(result.projected, balanced_amounts)
        self.assert_balancing_preserves_total(result)
        self.assertFalse(any(result.purchases.values()))
        self.assertEqual(result.alerts, [])

    def test_cash_deposit_restores_underweight_stock_buckets_to_target(self) -> None:
        """Invest excess cash into underweight stock categories until targets are restored."""
        expected_balanced_amounts: dict[str, Decimal] = self.target_amounts()
        starting_amounts: dict[str, Decimal] = dict(expected_balanced_amounts)
        shortfalls: dict[str, Decimal] = {
            US_BROAD_MARKET: expected_balanced_amounts[US_BROAD_MARKET] / 4,
            EMERGING_MARKETS: expected_balanced_amounts[EMERGING_MARKETS] / 4,
            INTERNATIONAL_MARKETS: expected_balanced_amounts[INTERNATIONAL_MARKETS] / 4,
        }
        for category, shortfall in shortfalls.items():
            starting_amounts[category] -= shortfall
        starting_amounts[CASH] += sum(shortfalls.values())

        # Excess cash exactly covers the configured stock-category shortfalls.
        result: Analysis = self.analyze_category_amounts(starting_amounts)

        self.assert_category_amounts(result.current, starting_amounts)
        self.assertEqual(sum(result.current.values()), PORTFOLIO_TOTAL)
        self.assertEqual(result.investable_cash, sum(shortfalls.values()))
        expected_purchases: dict[str, Decimal] = {
            key: Decimal("0") for key in self.categories
        }
        expected_purchases.update(shortfalls)
        self.assert_category_amounts(result.purchases, expected_purchases)
        self.assert_category_amounts(result.projected, expected_balanced_amounts)
        self.assert_balancing_preserves_total(result)
        self.assertEqual(result.alerts, [])

    def test_limited_cash_improves_large_imbalances_without_reaching_targets(self) -> None:
        """Use insufficient excess cash to reduce severe drift within all warning tolerances."""
        target_amounts: dict[str, Decimal] = self.target_amounts()
        starting_amounts: dict[str, Decimal] = dict(target_amounts)
        underweight_categories: tuple[str, ...] = (
            EMERGING_MARKETS,
            INTERNATIONAL_MARKETS,
            REAL_ESTATE,
            BONDS,
        )
        shortfall_fraction: Decimal = Decimal("0.90")
        shortfalls: dict[str, Decimal] = {
            category: target_amounts[category] * shortfall_fraction
            for category in underweight_categories
        }
        us_tolerance: Decimal = (
            Decimal("0.01")
            + Decimal("0.10") * self.categories[US_BROAD_MARKET].target
        )
        us_overweight: Decimal = PORTFOLIO_TOTAL * us_tolerance

        starting_amounts[US_BROAD_MARKET] += us_overweight
        for category, shortfall in shortfalls.items():
            starting_amounts[category] -= shortfall
        starting_amounts[CASH] += sum(shortfalls.values()) - us_overweight

        result: Analysis = self.analyze_category_amounts(starting_amounts)

        self.assert_balancing_preserves_total(result)
        self.assertTrue(any(result.purchases.values()))
        self.assertLess(result.investable_cash, sum(shortfalls.values()))
        for category in underweight_categories:
            self.assertLess(result.projected[category], target_amounts[category])
        self.assertNotEqual(result.projected, target_amounts)
        self.assertEqual(result.alerts, [])

    def test_existing_overweight_warns_without_recommending_a_sale(self) -> None:
        """Warn about an overweight category without proposing a sale or new purchases."""
        starting_amounts: dict[str, Decimal] = self.target_amounts()
        emerging_shortfall: Decimal = starting_amounts[EMERGING_MARKETS] / 2
        international_shortfall: Decimal = starting_amounts[INTERNATIONAL_MARKETS] / 2
        starting_amounts[US_BROAD_MARKET] += emerging_shortfall + international_shortfall
        starting_amounts[EMERGING_MARKETS] -= emerging_shortfall
        starting_amounts[INTERNATIONAL_MARKETS] -= international_shortfall
        result: Analysis = self.analyze_category_amounts(starting_amounts)

        self.assert_category_amounts(result.current, starting_amounts)
        self.assert_category_amounts(result.projected, starting_amounts)
        self.assert_balancing_preserves_total(result)
        self.assertFalse(any(result.purchases.values()))
        self.assertEqual(result.unallocated_cash, Decimal("0.00"))
        self.assertTrue(any(self.categories[US_BROAD_MARKET].label in alert for alert in result.alerts))

    def test_hybrid_tolerance_boundary_is_not_an_alert(self) -> None:
        """Allow a category whose drift is exactly at the configured tolerance boundary."""
        starting_amounts: dict[str, Decimal] = self.target_amounts()
        target: Decimal = self.categories[US_BROAD_MARKET].target
        tolerance: Decimal = Decimal("0.01") + Decimal("0.10") * target
        boundary_amount: Decimal = PORTFOLIO_TOTAL * tolerance
        starting_amounts[US_BROAD_MARKET] += boundary_amount
        starting_amounts[CASH] -= boundary_amount

        # U.S. broad market is exactly at its configured 1% + 10%-of-target band.
        result: Analysis = self.analyze_category_amounts(starting_amounts)
        self.assert_balancing_preserves_total(result)
        self.assertFalse(
            any(self.categories[US_BROAD_MARKET].label in alert for alert in result.alerts)
        )


if __name__ == "__main__":
    unittest.main()