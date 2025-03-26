<template>
    <div>
      <canvas ref="chartCanvas"></canvas>
    </div>
  </template>
  
  <script>
  import { ref, onMounted } from "vue";
  import { Chart, registerables } from "chart.js";
  
  // Register Chart.js components
  Chart.register(...registerables);
  
  export default {
    props: {
      labels: {
        type: Array,
        required: true,
      },
      datasets: {
        type: Array,
        required: true, // Accept multiple datasets
      },
    },
    setup(props) {
      const chartCanvas = ref(null);
      let chartInstance = null;
  
      onMounted(() => {
        if (chartCanvas.value) {
          chartInstance = new Chart(chartCanvas.value, {
            type: "line",
            data: {
              labels: props.labels,
              datasets: props.datasets, // Use the datasets prop
            },
            options: {
              responsive: true,
              maintainAspectRatio: false,
              scales: {
                x: {
                  title: {
                    display: true,
                    text: "Time",
                  },
                },
                y: {
                  title: {
                    display: true,
                    text: "Value",
                  },
                  beginAtZero: true,
                },
              },
            },
          });
        }
      });
  
      return {
        chartCanvas,
      };
    },
  };
  </script>
  
  <style scoped>
  canvas {
    max-width: 100%;
    height: 400px;
  }
  </style>