import React from 'react';
import ChartWrapper from './ChartWrapper.jsx';
import { formatSecondsToMMSS } from '../utils/time.js';


function BanhoList({ border = false, banhos = [] }) {
  const banhoList = React.Children.toArray(banhos.map((banho, index) => (
    <div key={index}>
      <ChartWrapper
        dateTime={new Date(banho.data_criacao).toLocaleString()}
        volume={banho.volume_agua}
        duration={formatSecondsToMMSS(banho.duracao)}
        limit={banho.limite}
        inProgress={banho.em_andamento}
      />
    </div>
  )));

  return (
    <div>
      {banhoList.map((child, index) => (
        <div key={index} className={index < banhoList.length - 1 && border ? 'pt-2 pb-6 mb-4 border-b border-neutral-200' : ''}>
          {child}
        </div>
      ))}
    </div>
  );
}

export default BanhoList;