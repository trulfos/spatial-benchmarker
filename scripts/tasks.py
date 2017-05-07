#!/usr/bin/python3
import argparse
from database import Database


class TasksAction(argparse.Action):

    def parse_task(self, task):
        return tuple(map(int, task.split(':')))

    def fetch(self, db, suite_id):
        return set(
                (sb['config_id'], sb['benchmark_id'])
                for sb in db.get_where('suite_member', suite_id=suite_id)
            )

    def __call__(self, parser, namespace, values, option_string):
        if not hasattr(namespace, 'database'):
            raise Exception('Dabase argument must be present for task action')

        db = Database(namespace.database)
        value_list = values if self.nargs else [values]
        tasks = set()

        for value in value_list:
            if value.count(':') == 1:
                tasks.add(self.parse_task(value))
            else:
                tasks |= self.fetch(db, int(value))

        setattr(namespace, self.dest, tasks)
