# from .. import db


# class Banho(db.Model):
#     __tablename__ = 'banhos'

#     id = db.Column(db.Integer, primary_key=True)
#     duracao = db.Column(db.Integer, nullable=False, default=0)  # duração em minutos
#     volume_agua = db.Column(db.Float, nullable=False, default=0.0)  # volume de água em litros
#     data_hora = db.Column(db.DateTime, default=db.func.current_timestamp())

#     def to_dict(self):
#         return {
#             'id': self.id,
#             'duracao': self.duracao,
#             'volume_agua': self.volume_agua,
#             'data_hora': self.data_hora.isoformat()
#         }

from dataclasses import dataclass
import datetime
from uuid import uuid4


@dataclass
class Banho:
    _id: str  # uuid4
    duracao: int  # duração em minutos
    volume_agua: float  # volume de água em litros
    data_criacao: str  # ISO formatted date-time string
    data_modificacao: str | None  # ISO formatted date-time string
    limite: float  # limite de volume de água em litros
    em_andamento: bool = True  # indica se o banho está em andamento

    @classmethod
    def default(cls):
        return cls(
            _id=str(uuid4()),
            duracao=0,
            volume_agua=0.0,
            data_criacao=datetime.datetime.now(datetime.timezone.utc).isoformat(),
            data_modificacao=None,
            em_andamento=True,
            limite=1.0
        )

    def to_dict(self):
        return {
            'id': self._id,
            'duracao': self.duracao,
            'volume_agua': self.volume_agua,
            'data_criacao': self.data_criacao,
            'data_modificacao': self.data_modificacao,
            'em_andamento': self.em_andamento,
            'limite': self.limite
        }

    @classmethod
    def from_db(cls, row):
        return cls(
            _id=row[0],
            data_criacao=row[1],
            data_modificacao=row[2],
            duracao=row[3],
            volume_agua=row[4],
            em_andamento=bool(row[5]),
            limite=row[6]
        )
