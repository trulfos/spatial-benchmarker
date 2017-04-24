#!/usr/bin/python3
import os
import sqlite3


class Database:
    """
    Represents a results database
    """

    def __init__(self, filename=None):
        """
        Opens database with the given filename (or default filename if none is
        given).
        """
        if filename is None:
            filename = 'results'

        # Check if database exists and create it if not
        exists = os.path.exists(filename)
        self.connection = sqlite3.connect(filename)
        self.connection.row_factory = sqlite3.Row

        if not exists:
            self.create_database()

    def create_database(self):
        """
        Creates the necessary tables
        """
        statements = [
                """
                create table `config` (
                    `config_id` integer primary key autoincrement,
                    `index` text not null
                )
                """,
                """
                create table `run` (
                    `run_id` integer primary key autoincrement,
                    `benchmark_id` integer not null,
                    `config_id` integer not null,
                    `timestamp` datetime default current_timestamp not null,
                    `commit` text not null,

                    foreign key (`benchmark_id`)
                        references `benchmark` (`benchmark_id`),
                    foreign key (`config_id`)
                        references `config` (`config_id`)
                )
                """,
                """
                create table `option` (
                    `config_id` integer not null,
                    `name` text not null,
                    `value` text not null,

                    primary key (`config_id`, `name`),
                    foreign key (`config_id`) references `config` (`config_id`)
                )
                """,
                """
                create table `benchmark` (
                    `benchmark_id` integer primary key autoincrement,
                    `dataset` text not null
                )
                """,
                """
                create table `reporter` (
                    `reporter_id` integer primary key autoincrement,
                    `benchmark_id` integer not null,
                    `name` text not null,
                    `arguments` text not null,

                    unique (`benchmark_id`, `name`, `arguments`),
                    foreign key (`benchmark_id`)
                        references `benchmark` (`benchmark_id`)
                )
                """,
                """
                create table `result` (
                    `run_id` integer not null,
                    `reporter_id` integer not null,
                    `name` text not null,
                    `value` real not null,

                    foreign key (`run_id`) references `run` (`run_id`),
                    foreign key (`reporter_id`)
                        references `reporter` (`reporter_id`)
                )
                """,
                """
                create table `suite` (
                    `suite_id` integer primary key autoincrement,
                    `name` text not null
                )
                """,
                """
                create table `suite_member` (
                    `suite_id` integer not null,
                    `benchmark_id` integer not null,
                    `config_id` integer not null,

                    foreign key (`suite_id`) references `suite` (`suite_id`),
                    foreign key (`benchmark_id`)
                        references `benchmark` (`benchmark_id`),
                    foreign key (`config_id`)
                        references `config` (`config_id`)
                )
                """,
                """
                create view `latest_run` as
                select *, cast(substr(`dataset`, -2) as integer) `D` from `run`
                inner join `benchmark` using (`benchmark_id`)
                inner join `config` using (`config_id`)
                where not exists (
                    select * from `run` `r`
                    where `r`.`timestamp` > `run`.`timestamp`
                    and `r`.`benchmark_id` = `run`.`benchmark_id`
                )
                """
            ]

        for s in statements:
            self.connection.execute(s)

    def create_insert_statement(self, table, keys):
        return 'insert into `%s` (%s) values (%s)' % (
                table,
                ', '.join('`%s`' % k for k in keys),
                ', '.join(':%s' % k for k in keys)
            )

    def insert(self, table, **kwargs):
        """
        Inserts a row in the given table.
        """
        return self.connection.execute(
                self.create_insert_statement(table, kwargs.keys()),
                kwargs
            )

    def insertmany(self, table, data):
        """
        Inserts all rows given in the table
        """
        data = list(data)

        if len(data) < 1:
            return

        return self.connection.executemany(
                self.create_insert_statement(table, data[0].keys()),
                data
            )

    def commit(self):
        self.connection.commit()

    def get_by_id(self, table, id):
        return self.connection.execute(
                'select * from `%s` where `%s_id` = ?' % (table, table),
                [id]
            ).fetchone()

    def get_where(self, table, **kwargs):
        where = ' and '.join('`%s` = :%s' % (k, k) for k in kwargs.keys())

        return self.connection.execute(
                'select * from `%s` where %s' % (table, where),
                kwargs
            ).fetchall()

    def get_all(self, table):
        return self.connection.execute(
                'select * from `%s`' % table
            ).fetchall()
