import time

from django.core.management.base import BaseCommand
from app import datacollectviews


class Command(BaseCommand):
    help = "统计用户访问数据"

    def add_arguments(self, parser):
        # parser.add_argument("poll_ids", nargs="+", type=int)
        pass

    def handle(self, *args, **options):
        datacollectviews.summary_visit_list()
