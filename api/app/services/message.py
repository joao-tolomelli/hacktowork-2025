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
    def notify_limite(level: str):
        r.publish("chuveiro:limite:notifications", level)

from .banho import BanhoService
