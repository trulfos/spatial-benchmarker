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

    # Calculate averages
    averages = db.connection.execute(
            """
            select `index`, `dataset`, avg(`value`) `value`
            from `latest_run`
            inner join `result` using (`run_id`)
            where `name` = ?
            group by `index` , `dataset`
            """,
            [args.metric]
        ).fetchall()

    names = {}
    fields = set()

    for a in averages:
        name = a['dataset']
        index = a['index']

        if name not in names:
            names[name] = {'dataset': name}

        names[name][index] = '%.5e' % a['value']
        fields |= {index}

    writer = csv.DictWriter(sys.stdout, fieldnames=['dataset'] + list(fields))
    writer.writeheader()
    writer.writerows(
            sorted(
                    names.values(),
                    key=lambda x: (int(x['dataset'][-2:]), x['dataset'])
                )
        )


main()
