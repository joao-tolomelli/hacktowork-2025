import { Clock, Droplet } from 'lucide-react';
import BulletChart from '../charts/BulletChart.jsx';

function ChartWrapper({ dateTime, volume = 0, duration = '00:00', limit = 50, inProgress = false }) {
  return (
    <div className="max-w-[88ch] h-20 mb-6">
    <div className="flex justify-end first:justify-between items-center">
        <span className="text-neutral-600 text-sm">{dateTime}</span>
        <span className={`text-md ${inProgress ? 'text-green-600' : 'text-neutral-600'}`}>{inProgress ? 'Em Andamento' : 'Concluído'}</span>
        <div className="flex items-center space-x-4 mr-4 border border-neutral-300 pr-4 rounded-lg bg-neutral-100 px-2 py-1">
          <span className="text-neutral-800 text-md flex"><Droplet color="#4A90E2" /> {volume.toFixed(1)} L</span>
          <span className="text-neutral-800 text-md flex"><Clock color="darkgray" className="mr-1" /> {duration}</span>
        </div>
        </div>
        <BulletChart value={volume} limit={limit} />
    </div>
  );
}

export default ChartWrapper;