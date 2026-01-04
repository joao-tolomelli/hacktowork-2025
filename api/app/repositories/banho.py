import datetime
from ..models.banho import Banho
import sqlite3


class BanhoRepository:
    def __init__(self, redis_client):
        self.redis_client = redis_client
        self.conn = sqlite3.connect('hacktowork.db')
        self.cursor = self.conn.cursor()
        self.create_tables()

    def create_tables(self):
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS banhos (
                id TEXT PRIMARY KEY,
                data_criacao TEXT,
                data_modificacao TEXT,
                duracao INTEGER,
                volume_agua REAL,
                em_andamento INTEGER DEFAULT 1,
                limite REAL,
                UNIQUE(id)
            )
        ''')
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS banho_limits (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                limite REAL,
                data_criacao TEXT
            )
        ''')
        if not self.get_banho_limit():
            default_limit = 50.0  # valor padrão inicial
            data_criacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
            self.cursor.execute('''
                INSERT INTO banho_limits (limite, data_criacao)
                VALUES (?, ?)
            ''', (default_limit, data_criacao))
        self.conn.commit()

    def get_banho_limit(self, _id: int | None = None) -> float | None:
        if _id is not None:
            limit = self.cursor.execute('SELECT limite FROM banho_limits WHERE id = ?', (_id,)).fetchone()
        else:
            limit = self.cursor.execute('SELECT limite FROM banho_limits ORDER BY data_criacao DESC LIMIT 1').fetchone()
        if limit:
            return limit[0]
        return None

    def create_banho(self) -> Banho:
        _banho = Banho.default()
        _limite = self.get_banho_limit()
        self.cursor.execute('''
            INSERT INTO banhos (id, data_criacao, data_modificacao, duracao, volume_agua, em_andamento, limite)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (_banho._id, _banho.data_criacao, _banho.data_modificacao, _banho.duracao, _banho.volume_agua, _banho.em_andamento, _limite))
        self.conn.commit()
        return _banho

    def get_all_banhos(self) -> list[Banho]:
        self.cursor.execute('SELECT id, data_criacao, data_modificacao, duracao, volume_agua, em_andamento, limite FROM banhos')
        rows = self.cursor.fetchall()
        banhos = [Banho.from_db(row) for row in rows]
        return banhos

    def get_banho_by_id(self, banho_id) -> Banho | None:
        banho = self.cursor.execute('SELECT id, data_criacao, data_modificacao, duracao, volume_agua, em_andamento, limite FROM banhos WHERE id = ?', (banho_id,)).fetchone()
        if banho:
            return Banho.from_db(banho)
        return None

    def get_latest_banho(self) -> Banho | None:
        banho = self.cursor.execute('SELECT id, data_criacao, data_modificacao, duracao, volume_agua, em_andamento, limite FROM banhos ORDER BY data_criacao DESC LIMIT 1').fetchone()
        if banho:
            return Banho.from_db(banho)
        return None

    def update_default_limit(self, limite: float) -> None:
        data_criacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
        self.cursor.execute('''
            INSERT INTO banho_limits (limite, data_criacao)
            VALUES (?, ?)
        ''', (limite, data_criacao))
        self.conn.commit()

    def update_banho_limit(self, banho_id, limite: float) -> None:
        banho = self.get_banho_by_id(banho_id)
        if not banho:
            return None
        banho.limite = limite
        banho.data_modificacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
        self.cursor.execute('''
            UPDATE banhos
            SET data_modificacao = ?, limite = ?
            WHERE id = ?
        ''', (banho.data_modificacao, banho.limite, banho_id))
        self.conn.commit()

    def update_duracao(self, banho_id, duracao=None) -> None:
        banho = self.get_banho_by_id(banho_id)
        if not banho:
            return None
        if duracao is not None:
            banho.duracao = duracao
        banho.data_modificacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
        self.cursor.execute('''
            UPDATE banhos
            SET data_modificacao = ?, duracao = ?
            WHERE id = ?
        ''', (banho.data_modificacao, banho.duracao, banho_id))
        self.conn.commit()

    def finalize_banho(self, banho_id) -> None:
        banho = self.get_banho_by_id(banho_id)
        if not banho:
            return None
        banho.em_andamento = False
        banho.data_modificacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
        self.cursor.execute('''
            UPDATE banhos
            SET data_modificacao = ?, em_andamento = ?
            WHERE id = ?
        ''', (banho.data_modificacao, 0, banho_id))
        self.conn.commit()

    def update_volume_agua(self, banho_id, volume_agua=None) -> None:
        banho = self.get_banho_by_id(banho_id)
        if not banho:
            return None
        if volume_agua is not None:
            banho.volume_agua = volume_agua
        banho.data_modificacao = datetime.datetime.now(datetime.timezone.utc).isoformat()
        self.cursor.execute('''
            UPDATE banhos
            SET data_modificacao = ?, volume_agua = ?
            WHERE id = ?
        ''', (banho.data_modificacao, banho.volume_agua, banho_id))
        self.conn.commit()
