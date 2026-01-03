import datetime
from .. import r


class MessageService:
    @staticmethod
    def handle_telemetria(message):
        data = message["data"].decode()
        BanhoService.store_telemetria(data)

    @staticmethod
    def handle_status(message):
        data = message["data"].decode()
        BanhoService.store_status(data)

    @staticmethod
    def notify_limite():
        r.publish("chuveiro:limite:notifications", datetime.datetime.now(datetime.timezone.utc).isoformat())

from .banho import BanhoService
