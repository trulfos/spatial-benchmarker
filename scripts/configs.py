#!/usr/bin/python3
import argparse
from database import Database


def create(db, index, definitions):
    if get(db, index, definitions):
        raise Exception("This config already exists")

    config_id = db.insert('config', index=index).lastrowid
    db.insertmany(
            'option',
            ({'name': d[0], 'value': d[1], 'config_id': config_id}
                for d in definitions.items())
        )

    db.commit()

    return config_id


def group(records):
    groups = {}

    for r in records:
        id = r[0]
        if id not in groups:
            groups[id] = {'id': id, 'index': r[1], 'definitions': {}}

        groups[id]['definitions'][r[2]] = r[3]

    return list(groups.values())


def get(db, index, options):
    where_clause = ' or '.join(['(`name` = ? and `value` = ?)'] * len(options))

    # Collect all params
    params = [index]
    for o in options.items():
        params += o

    params += [len(options)]

    # Perform the search
    rows = db.connection.execute(
            """
            with `relevant` as (
                select `config`.`id`, count(*) as c
                from `config`
                inner join `option` on `option`.`config_id` = `config`.`id`
                where `index` = ? and (%s)
                group by `config`.`id`
                having c = ?
            )
            select `id`, `name`, `value`
            from `relevant`
            inner join `option`
                on `option`.`config_id` = `relevant`.`id`
            """ % where_clause,
            params
        ).fetchall()

    return group(rows)


def get_by_id(db, id):
    results = get_all(db, id=id)

    if not results:
        return None

    return results[0]


def get_all(db, **kwargs):
    where_clause = ''

    if kwargs:
        where_clause = 'where %s' % (
                ' and '.join('`%s` = ?' % k for k in kwargs.keys())
            )

    rows = db.connection.execute(
            """
            select `id`, `index`, `name`, `value`
            from `config`
            inner join `option`
                on `option`.`config_id` = `config`.`id`
            """ + where_clause,
            list(kwargs.values())
        ).fetchall()

    return group(rows)


def print_all(records):
    for r in records:
        print(
                "-- Config %d (%s) --" % (r['id'], r['index']) +
                ''.join("\n %s:\t%s" % c for c in r['definitions'].items()) +
                '\n'
            )


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Retrieve and create configs from results database'
        )

    parser.add_argument(
            '--create', '-c', action='store_true',
            help='Create a new config'
        )

    parser.add_argument(
            'config', nargs='*',
            help='Cofig id or specification'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def main():
    args = parse_arguments()
    db = Database(args.database)

    if not args.config:
        print_all(get_all(db))
        return

    index = args.config[0]
    definitions = dict(d.split('=') for d in args.config[1:])

    if args.create:
        print("New config with id %d" % create(db, index, definitions))
    elif index.isdecimal():
        if len(definitions):
            print("Warning: Unused arguments")

        result = get_by_id(db, int(index))

        if result:
            print_all([result])
        else:
            print("No such config")
    elif definitions:
        print_all(get(db, index, definitions))
    else:
        print_all(get_all(db, index=index))


if __name__ == '__main__':
    main()
