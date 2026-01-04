import BorderedList from './components/BorderedList.jsx';
import BulletChart from './charts/BulletChart.jsx';
import ChartWrapper from './components/ChartWrapper.jsx';

function App() {

  return (
    <div className="w-screen bg-neutral-100 min-h-screen h-full">
      <div className="flex flex-col items-center h-full">
        <div className="w-[88ch] bg-neutral-50 p-4 rounded-lg shadow-md my-8">
          <h2 className="text-lg font-semibold mb-2">Banhos</h2>
          <BorderedList border={true}>
            <ChartWrapper dateTime="03/01/2026 22:23" volume={20} duration="13:35">
              <BulletChart value={20} />
            </ChartWrapper>
            <ChartWrapper>
              <BulletChart value={50} />
            </ChartWrapper>
            <ChartWrapper>
              <BulletChart value={75} />
            </ChartWrapper>
            <ChartWrapper>
              <BulletChart value={95} />
            </ChartWrapper>
          </BorderedList>
        </div>
      </div>
    </div>
  );
}

export default App;
