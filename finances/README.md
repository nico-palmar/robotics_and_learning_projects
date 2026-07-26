# Schwab Stock Balancer

This local, advisory-only script reads a Schwab **Individual Positions** CSV export and recommends how to invest cash by allocation category. It does not connect to Schwab, choose a fund, place trades, recommend sales, or give tax advice.

## Targets

| Category | Target |
| --- | ---: |
| U.S. broad market | 35% |
| Emerging markets | 17.5% |
| International markets | 17.5% |
| Real estate (REITs) | 10% |
| Bonds and bond substitutes | 15% |
| Cash and money market | 5% |

The starting mappings are `SWTSX`, `SCHE`, `SWISX`, `SCHH`, `CMF`, and `SWVXX`. Edit [category_mappings.json](category_mappings.json) if allocations change.

## Run

From the `finances` directory, run:

    python schwab_stock_balancer.py /path/to/schwab_positions.csv

The script locates the `Symbol` header itself, reads the `Mkt Val (Market Value)` column, includes Schwab's `Cash & Cash Investments` balance as cash, and excludes the `Positions Total` summary row. A valid export needs actual market values; the supplied schema-only CSV intentionally cannot be analyzed.

If a position symbol is new, the script asks for one configured category and saves the answer to [category_mappings.json](category_mappings.json). It never silently omits an unknown position.

## Recommendations and alerts

The tool retains 5% of the **current total portfolio value** as cash. It recommends investing only cash above that reserve, into underweight non-cash categories in proportion to their dollar shortfalls. If overweights make a sensible purchase impossible, it leaves the remaining cash alone and explains why.

The `After buy %` column shows the portfolio allocation after following the recommendations, even when the available cash cannot reach every target. The percentage beside each recommended purchase is that category's share of the cash being invested, not a percentage of the total portfolio.

The report ends with the configured category-to-symbol mappings, grouped by category, to identify the available investments for each allocation bucket.

After the recommendation, it shows current and projected allocations. It warns for any category still beyond this first-pass hybrid drift band:

$$\text{tolerance} = 1\text{ percentage point} + 10\% \times \text{target allocation}$$

This tolerance is deliberately marked for future evaluation in the implementation.