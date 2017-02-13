#!/usr/bin/python3
import sqlite3
import argparse
import csv
import sys


def fetch(connection, args):

    if ',' in args.group:
        ids = ', '.join(i for i in args.group.split(','))

        cursor = connection.execute(
                """
                with `relevant_runs` as (
                        select `id`, `commit`, `timestamp`, `config_id`
                        from `run`
                        where `config_id` in (%s)
                    )
                select
                    `r1`.`commit`,
                    `r1`.`config_id`,
                    avg(`result`.`value`)
                from `relevant_runs` `r1`
                outer left join `relevant_runs` `r2`
                    on `r1`.`timestamp` < `r2`.`timestamp` and
                        `r1`.`config_id` = `r2`.`config_id`
                inner join `result` on `result`.`run_id` = `r1`.`id`
                where
                    `r2`.`id` is null and
                    `metric` = :metric
                group by `r1`.`config_id`
                """ % ids,
                args.__dict__
            )
    else:
        # Retrieve config ids
        consts = {a.split('=') for a in args.group}
        options = connection.execute(
                'select distinct `name` from `option`'
            ).fetchall()

        names = set(o[0] for o in options)
        variables = names - set(consts.keys())

        filter_clause = ' and '.join(
                ['`option`.`name` = ? and `option`.`value` = ?'] * len(consts)
            )

        """
        with `config_ids` as (
            select `config`.`id`
            from `config`
            inner join `option`
                on `option`.`config_id` = `config`.`id`
            where %s
        )
        with `relevant_runs` as (
            select `run`.`id`
            from 
        )
        """ % filter_clause

        cursor = connection.execute(
                """
                with relevant_runs as (
                        select
                            `run`.`id` `id`,
                            `run`.`commit` `commit`,
                            `run`.`timestamp` `timestamp`,
                            `run`.`index` `index`,
                            `option`.`value` `value`
                        from `run`
                            inner join `config`
                                on `config`.`id` = `run`.`config_id`
                            inner join `option`
                                on `config`.`id` = `option`.`config_id`
                        where
                            %s and
                            exists (
                                select * from `result`
                                where
                                    `run_id` = `run`.`id` and
                                    `metric` = :metric
                            )
                    )
                select
                    `r1`.`commit`,
                    `r1`.`value`,
                    `r1`.`index`,
                    avg(`result`.`value`)
                from relevant_runs `r1`
                outer left join relevant_runs `r2`
                    on `r1`.`timestamp` < `r2`.`timestamp` and
                        `r1`.`value` = `r2`.`value` and
                        `r1`.`index` = `r2`.`index`
                inner join `result` on `result`.`run_id` = `r1`.`id`
                where
                    `r2`.`id` is null and
                    `metric` = :metric
                group by `r1`.`value`, `r1`.`index`
                """ % filter_clause,
                args.__dict__
            )

    # Fetch results
    results = cursor.fetchall()

    # Check commit hashes
    commits = set(r[0] for r in results)
    if len(commits) > 1:
        print(
                "\033[31mWARNING:\033[0m Results from different commits!",
                file=sys.stderr
            )

    # Strip commit field
    return [r[1:] for r in results]


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Extract results from SQLite file'
        )

    parser.add_argument(
            'metric',
            help='Name of metric to plot (y-axis)'
        )

    parser.add_argument(
            'group', metavar='<option/configs>',
            help='Group by given property (x-axis)'
        )

    parser.add_argument(
            '--benchmark', '-b',
            help='Select specific benchmark.'
        )

    parser.add_argument(
            '--aggregate', '-a', default='avg',
            help='Aggregate function for aggregating results within group'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename',
            help='Path to sqlite database file'
        )

    parser.add_argument(
            '--plot', '-p', action='store_true',
            help='Plot the result immediately using pyplot'
        )

    return parser.parse_args()


def group(tuples):
    groups = {}
    fields = set()

    for t in tuples:
        name = t[0]

        fields.add(name)

        if name not in groups:
            groups[name] = {'key': name}

        groups[name][t[1]] = t[2]

    # Sort as we have messed up the order when grouping
    return [
            ['key'] + sorted(list(fields)),
            sorted(list(groups.values()), key=lambda r: int(r['key']))
        ]


def plot(keys, results):
    import matplotlib.pyplot as pyplot

    if len(keys) == 2:
        pyplot.bar(
                range(0, len(results)),
                [a['value'] for a in results],
                tick_label=list(a['key'] for a in results)
            )
    else:
        indexes = set()

        for r in results:
            indexes |= r.keys()

        indexes.discard('key')

        for k in indexes:
            pyplot.plot(
                    [
                        results[i]['key'] for i in range(0, len(results))
                        if k in results[i]
                    ],
                    [a[k] for a in results if k in a],
                    label=k
                )
            pyplot.legend()

    pyplot.show()


def main():
    args = parse_arguments()
    connection = sqlite3.connect(args.database or 'results')

    # Fetch rows
    results = fetch(connection, args)

    # Restructure and find keys
    if not len(results):
        return

    if len(results[0]) > 2:
        [keys, results] = group(results)
    else:
        results = [{'key': r[0], 'value': r[1]} for r in results]
        keys = ['key', 'value']

    # Write results in tab separated csv format
    writer = csv.DictWriter(
            sys.stdout,
            fieldnames=results[0].keys(),
            delimiter='\t'
        )

    writer.writeheader()
    writer.writerows(results)

    # Plot and exit if requested
    if args.plot:
        plot(keys, results)


main()
