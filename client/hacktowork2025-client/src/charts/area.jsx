import { Line } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler,
} from 'chart.js';

// Register Line-specific elements
ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
);

const AreaChart = ({
  labels,
  datasetLabel,
  dataPoints,
  title,
  threshold = 50, // Default threshold value
}) => {
  const data = {
    labels: labels,
    datasets: [
      {
        fill: true,
        label: datasetLabel,
        data: dataPoints,
        borderColor: dataPoints.map((point) =>
          point > threshold ? 'rgb(255, 99, 132)' : 'rgb(75, 192, 192)'
        ),
        backgroundColor: dataPoints.map((point) =>
          point > threshold ? 'rgba(255, 99, 132, 0.5)' : 'rgba(75, 192, 192, 0.5)'
        ),
        tension: 0.3,
      },
    ],
  };

  const options = {
    responsive: true,
    plugins: {
      legend: {
        position: 'top',
      },
      title: {
        display: true,
        text: title,
      },
    },
    scales: {
      y: {
        beginAtZero: true,
      },
    },
  };

  // Custom plugin to draw a horizontal line for the threshold
  const thresholdPlugin = {
    id: 'thresholdPlugin',
    beforeDraw: (chart) => {
      const {
        ctx,
        chartArea: { top, bottom, left, right },
        scales: { y },
      } = chart;

      ctx.save();
      ctx.beginPath();
      ctx.lineWidth = 2;
      ctx.strokeStyle = 'red'; // Color of the threshold line
      ctx.setLineDash([5, 5]); // Dashed line
      const yThreshold = y.getPixelForValue(threshold); // Get the pixel position for the threshold value
      ctx.moveTo(left, yThreshold);
      ctx.lineTo(right, yThreshold);
      ctx.stroke();
      ctx.restore();
    },
  };

  // Register the plugin
  ChartJS.register(thresholdPlugin);

  return <Line data={data} options={options} />;
};

export default AreaChart;
