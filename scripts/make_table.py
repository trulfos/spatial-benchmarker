#!/usr/bin/python3
from database import Database
import argparse


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Make a table similar to the one from the RR* article'
        )

    parser.add_argument(
            'index', metavar='<index name>',
            help='Index for which to generate the table'
        )

    parser.add_argument(
            'suite', metavar='<suite_id>',
            help='Suite to collect numbers from'
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

    # Identify relevant runs
    benchmark_runs = db.connection.execute(
            """
            select * from `latest_run`
            inner join `suite_member` using (`benchmark_id`, `config_id`)
            where `index` = ? and `suite_id` = ? and exists (
                    select * from reporter
                    where `benchmark_id` = `latest_run`.`benchmark_id` and
                        `name` in ('struct', 'stats', 'correctness')
                )
            """,
            [args.index, args.suite]
        ).fetchall()

    benchmark_runs.sort(key=lambda x: x['dataset'])

    values = ['benchmark'] \
        + 3 * ['results', 'leafAcc'] \
        + ['Leafs'] + (
                ['Perimspls'] if args.index == 'rtree-rstar' else []
            ) + ['commit', 'id']

    # TODO: Guarantee sort order?
    for run in benchmark_runs:
        values.append(run['dataset'].split('/')[-1])

        reporters = db.connection.execute(
                """
                select * from reporter
                where `benchmark_id` = ? and
                `name` in ('struct', 'stats', 'correctness')
                """,
                (run['benchmark_id'],)
            )

        for reporter in reporters:
            results = to_dict(db.get_where(
                    'result',
                    run_id=run['run_id'],
                    reporter_id=reporter['reporter_id']
                ))

            if reporter['name'] == 'stats':
                values += [
                        '%.2e' % results['results'],
                        '%.2e' % results['leaf_accesses']
                    ]
            elif reporter['name'] == 'struct':
                h = results['height']
                values.append('%.2e' % (results['level_2'] / 1000))

                if args.index == 'rtree-rstar':
                    values.append(
                            '%.2e' % (
                                100*results['perimeter_splits'] /
                                (results['nodes'] - h)
                            )
                        )
            elif reporter['name'] == 'correctness' and len(results):
                print('Incorrect results detected! Go hunt more bugs.')
                exit(1)

        values += [
                run['commit'].decode('utf-8')[:6],
                '%d' % run['benchmark_id']
            ]

    width = len(benchmark_runs) + 1
    height = len(values) // width

    for i in range(0, len(values) // width):
        print('\t'.join(v for v in values[i::height]))


main()
