import React from 'react';
import { Bar } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Tooltip,
} from 'chart.js';
import annotationPlugin from 'chartjs-plugin-annotation';

// Register the components and the annotation plugin
ChartJS.register(CategoryScale, LinearScale, BarElement, Tooltip, annotationPlugin);

function BulletChart({ value, target = 80 }) {
  const getBarColor = (val) => {
    if (val >= 90) return '#d63031'; // Deep Red for critical
    if (val >= 70) return '#fdcb6e'; // Bright Gold for warning
    return '#0984e3'; // Strong Blue for healthy
  };

  const data = {
    labels: [''],
    datasets: [
      {
        data: [value],
        backgroundColor: getBarColor(value),
        borderColor: 'rgba(255, 255, 255, 0.8)',
        borderWidth: 1,
        barThickness: 24,
        borderRadius: 4, // Rounds the bar edges for a modern look
      },
    ],
  };

  const options = {
    indexAxis: 'y', // Makes it horizontal,
    maintainAspectRatio: false,
    scales: {
      x: {
        min: 0,
        max: 100,
        grid: { display: false  },
      },
      y: {
        grid: { display: false },
      },
    },
    plugins: {
      legend: { display: false },
      annotation: {
        annotations: {
          // 1. Background "Healthy" Zone
          lowZone: {
            drawTime: 'beforeDatasetsDraw',
            type: 'box',
            xMin: 0, xMax: 60,
            backgroundColor: 'rgba(46, 204, 113, 0.15)', // Light Mint
          },
          // 2. Background "Warning" Zone
          midZone: {
            drawTime: 'beforeDatasetsDraw',
            type: 'box',
            xMin: 60, xMax: 85,
            backgroundColor: 'rgba(241, 196, 15, 0.15)', // Light Amber
          },
          // 3. Background "Critical" Zone
          highZone: {
            drawTime: 'beforeDatasetsDraw',
            type: 'box',
            xMin: 85, xMax: 100,
            backgroundColor: 'rgba(231, 76, 60, 0.15)', // Light Coral
          },
        },
      },
    },
  };

  return <Bar data={data} options={options} />;
};

export default BulletChart;