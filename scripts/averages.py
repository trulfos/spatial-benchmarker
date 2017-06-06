#!/usr/bin/python3
from database import Database
import argparse
import csv
import sys

"""
This scripts calculates the averages from latest runs.
"""


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Calculate averages for all results of latest runs'
        )

    parser.add_argument(
            'suite', metavar='suite_id', nargs='+',
            help='Suite to gather averages from'
        )

    parser.add_argument(
            '--metric', '-m', metavar='<metric>', default='leaf_accesses',
            help='Metric to calculate averages for'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def to_dict(results):
    return dict((r['name'], r['value']) for r in results)


def main():
    args = parse_arguments()
    db = Database(args.database)

    db.connection.execute(
            """
            create temporary view `annotated_run` as
                select
                    `run_id`,
                    `index`,
                    cast(substr(`dataset`, -2) as integer) `D`
                from latest_run
                inner join suite_member using (`config_id`, `benchmark_id`)
                where `suite_id` in (%s)
            """ % ', '.join(args.suite)
        )

    # Calculate averages
    averages = db.connection.execute(
            """
            select
                `r1`.`index` `index`,
                `r1`.`D` `D`,
                avg(`value`) `value`

            from (
                select distinct `D`, `index` from `annotated_run`
            ) `r1`

            left outer join `annotated_run` `r2`
                on `r2`.`D` <= `r1`.`D`
                and `r2`.`index` = `r1`.`index`

            left outer join `result` using (`run_id`)

            where
                `name` = ?
            group by
                `r1`.`index`,
                `r1`.`D`
            """,
            [args.metric]
        ).fetchall()

    dimensions = {}
    fields = set()

    for a in averages:
        dimension = a['D']
        index = a['index']

        if dimension not in dimensions:
            dimensions[dimension] = {'dimension': dimension}

        dimensions[dimension][index] = '%.5e' % a['value']
        fields |= {index}

    writer = csv.DictWriter(
            sys.stdout,
            delimiter='\t',
            fieldnames=['dimension'] + sorted(list(fields))
        )

    writer.writeheader()
    writer.writerows(
            sorted(
                    dimensions.values(),
                    key=lambda x: x['dimension']
                )
        )


main()
