import { useEffect, useState } from 'react';
import { fetchBanhos } from './services/banho.js';
import BanhoList from './components/BanhoList.jsx';
import socketio from 'socket.io-client';

function App() {
  const [banhos, setBanhos] = useState([]);

  useEffect(() => {
    const fetchData = async () => {
      const data = await fetchBanhos();
      const _banhos = data.map((banho) => ({
        ...banho,
        data_criacao: new Date(banho.data_criacao),
      }));
      _banhos.sort((a, b) => b.data_criacao - a.data_criacao);
      setBanhos(_banhos);
    };
    fetchData();

    const io = socketio("http://localhost:5000");
    io.on('connect', () => {
      console.log('Connected to WebSocket server');
    });

    io.on('banho_update', (data) => {
      console.log('Received banho_update event', data);
      fetchData();
    });

    return () => {
      io.disconnect();
    };
  }, []);

  return (
    <div className="w-screen bg-neutral-100 min-h-screen h-full">
      <div className="flex flex-col items-center h-full">
        <div className="w-[88ch] bg-neutral-50 p-4 rounded-lg shadow-md my-8">
          <h2 className="text-lg font-semibold mb-2">Banhos</h2>
          <BanhoList border={true} banhos={banhos} />
        </div>
      </div>
    </div>
  );
}

export default App;
