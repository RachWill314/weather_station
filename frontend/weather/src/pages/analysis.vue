<template>
  <div class="image-background">
    <v-app-bar :elevation="0" class="transparent-app-bar" :color="transparent">
      <h3 style="text-align: left;">ELET2415 Weather Station</h3>
      <v-spacer></v-spacer>
      <v-btn class="rounded-button" :to="'/'">
        Home
      </v-btn>
      <v-btn class="rounded-button" :to="'/analysis'">
        Analysis
      </v-btn>
    </v-app-bar>
    <div class="content">
      <div class="columns-wrapper">
        <div class="column">
          <h2>Live Analysis</h2>
          <v-card>
            <v-tabs v-model="tab" bg-color="#7B84BF">
              <v-tab value="Temperature">Temperature & Heat Index</v-tab>
              <v-tab value="Humidity">Humidity & Soil Moisture</v-tab>
            </v-tabs>

            <v-card-text>
              <v-tabs-window v-model="tab">
                <v-tabs-window-item value="Temperature">
                  <div> <canvas style= "width: 800px;" id = "tempdata"> </canvas> </div>
                  <!-- <LineGraph :labels="timeLabels" :datasets="temperatureAndHeatIndexData" /> -->
                </v-tabs-window-item>

                <v-tabs-window-item value="Humidity">
                  <!-- <LineGraph :labels="timeLabels" :datasets="humidityAndSoilMoistureData" /> -->
                  <div> <canvas style= "width: 800px;" id = "soil"> </canvas> </div>

                </v-tabs-window-item>

              </v-tabs-window>
            </v-card-text>
          </v-card>
        </div>
        <div class="column">
          <h2>Analysis</h2>
          <v-card>
            <v-tabs v-model="tab2" bg-color="#df849c">
              <v-tab value="Temperature">Temperature, Heat Index & Humidity</v-tab>
            </v-tabs>
          </v-card>
          <div class="button-wrapper">
  <!-- Start Date Card -->
  <v-card class="date-card" outlined @click="startDateDialog = true">
    <v-card-text>
      <p class="date-card-title">Select Start Date</p>
    </v-card-text>
    <v-dialog v-model="startDateDialog" persistent max-width="290">
      <v-card>
        <v-date-picker
          v-model="startDate"
          @update:modelValue="startDateDialog = false"
        />
      </v-card>
    </v-dialog>
  </v-card>

  <!-- End Date Card -->
  <v-card class="date-card" outlined @click="endDateDialog = true">
    <v-card-text>
      <p class="date-card-title">Select End Date</p>
    </v-card-text>
    <v-dialog v-model="endDateDialog" persistent max-width="290">
      <v-card>
        <v-date-picker
          v-model="endDate"
          @update:modelValue="endDateDialog = false"
        />
      </v-card>
    </v-dialog>
  </v-card>

  <!-- Analyze Button -->
  <v-btn class="rounded-btn" @click="analyzeData">
    Analyze
  </v-btn>
</div>

        </div>
      </div>
    </div>
  </div>
</template>

<script setup >
import LineGraph from "@/components/LineGraph.vue";
import { storeToRefs } from "pinia";
import { useMqttStore } from "../stores/mqttStore"; // Import Mqtt Store
import { ref, reactive, watch, onMounted, onBeforeUnmount, computed, } from "vue";
import { Chart, registerables } from "chart.js";

Chart.register(...registerables);
const Mqtt = useMqttStore();

const { payload, payloadTopic, cardtitle, cardsubtitle, cardunit, cardunitconvert , tempData} =  storeToRefs(Mqtt);
const temperatureData = ref([]); // Example temperature data
    const heatIndexData = ref([]); // Example heat index data
    const humidityData = ref([]);
    const soilMoistureData = ref([]); // Example soil moisture data
     // Example humidity data
    const filteredLabels = ref([]); // Labels for the filtered graph
    const filteredGraphData = ref([]); 

onMounted(() => {
  makegraph();
  makesoilgraph();
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout(() => {
      Mqtt.subscribe("620155671");
      Mqtt.subscribe("620155671_pub");
      Mqtt.subscribe("620155671_sub");
    }, 3000);


    //CreateCharts();
  });
  
  onBeforeUnmount(() => {
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    Mqtt.unsubcribeAll();
  });

  let tempHiGraph =null;
  let soilhumchart = null;
const startDateDialog = ref(false); // Controls the visibility of the start date picker dialog
    const endDateDialog = ref(false); // Controls the visibility of the end date picker dialog
    const startDate = ref(null); // Stores the selected start date
    const endDate = ref(null); // Stores the selected end date
    const tab = ref("Temperature");
    const start = ref(null);
    const timeLabels = ref([]); // Example time labels

    // For analyze graph
    const dateLabels = ref([
      "2025-03-20",
      "2025-03-21",
      "2025-03-22",
      "2025-03-23",
      "2025-03-24",
    ]); // Example date labels

    const analyzeData = () => {
      if (!startDate.value || !endDate.value) {
        alert("Please select both start and end dates.");
        return;
      }

      // Convert dates to comparable format
      const start = new Date(startDate.value);
      const end = new Date(endDate.value);

    
    };

  watch(tab, (newTab) => {
  if (newTab === 'Temperature') {
    // Wait for DOM update
    nextTick(() => {
      if (!tempHiGraph) {
        makegraph();
      }
    });
  } else if (newTab === 'Humidity') {
    // Wait for DOM update
    nextTick(() => {
      if (!soilhumchart) {
        makesoilgraph();
      }
    });
  }
});

  watch(payload, (newPayload) => {
    console.log(heatIndexData.value);
    const now = new Date();
    const currentTime = now.toLocaleTimeString('en-US', { 
        hour12: false,
        hour: '2-digit', 
        minute: '2-digit'
    });

    // Update the time labels
    timeLabels.value = [...timeLabels.value, currentTime];
    if(timeLabels.value.length > 10) {
        timeLabels.value.shift();
    }
    // Update the temperature data
    temperatureData.value = [...temperatureData.value, newPayload.temperature];
    if(temperatureData.value.length > 10){
      temperatureData.value.shift();
    }

    heatIndexData.value = [...heatIndexData.value, newPayload.heatindex];
    if(heatIndexData.value.length > 10){
      heatIndexData.value.shift();
    }

    humidityData.value = [...humidityData.value, newPayload.humidity];
    if(humidityData.value.length > 10){
      humidityData.value.shift();
    }

    soilMoistureData.value = [...soilMoistureData.value, newPayload.soil];
    if(soilMoistureData.value.length > 10){
      soilMoistureData.value.shift();
    }


    if(tempHiGraph){
      tempHiGraph.data.labels = timeLabels.value;
      tempHiGraph.data.datasets[0].data = temperatureData.value;
      tempHiGraph.data.datasets[1].data = heatIndexData.value;
    tempHiGraph.update();
    }

    if(soilhumchart){
      soilhumchart.data.labels = timeLabels.value;
      soilhumchart.data.datasets[0].data = humidityData.value;
      soilhumchart.data.datasets[1].data = soilMoistureData.value;
    soilhumchart.update();
    }


    
    // Update the heat index data
    // // Update the humidity data
    // humidityData.value.push(newPayload.humidity);
    // // Update the time labels
    // timeLabels.value.push(newPayload.time);
  });

  const makegraph = () => {
    const tempCanvas = document.getElementById('tempdata');
  if (tempCanvas) {
    tempHiGraph = new Chart(tempCanvas, {
      type: 'line',
      data: {
        labels: timeLabels.value,
        datasets: [
          {
            label: "Temperature (°C)",
            data: temperatureData.value,
            borderColor: "rgba(75, 192, 192, 1)",
            backgroundColor: "rgba(75, 192, 192, 0.2)",
            borderWidth: 2,
            tension: 0.4,
          },
          {
            label: "Heat Index (°C)", 
            data: heatIndexData.value,
            borderColor: "rgba(255, 99, 132, 1)",
            backgroundColor: "rgba(255, 99, 132, 0.2)", 
            borderWidth: 2,
            tension: 0.4,
          }
        ]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: {
          duration: 0
        }
      }
    });
  }


};

const makesoilgraph = () => {
  const soilCanvas = document.getElementById('soil');
  if (soilCanvas) {
    soilhumchart = new Chart(soilCanvas, {
      type: 'line',
      data: {
        labels: timeLabels.value,
        datasets: [
          {
            label: "Humidity (%)",
            data: humidityData.value,
            borderColor: "rgba(75, 192, 192, 1)",
            backgroundColor: "rgba(75, 192, 192, 0.2)",
            borderWidth: 2,
            tension: 0.4,
          },
          {
            label: "Soil Moisture (%)", 
            data: soilMoistureData.value,
            borderColor: "rgba(255, 99, 132, 1)",
            backgroundColor: "rgba(255, 99, 132, 0.2)", 
            borderWidth: 2,
            tension: 0.4,
          }
        ]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: {
          duration: 0
        }
      }
    });
  }
}

</script>

<style scoped>
.image-background {
  position: relative;
  width: 100%;
  height: 100vh;
  /* Full viewport height */
  background-image: url('../assets/purplesky.jpg');
  /* Replace with your image path */
  background-size: cover;
  /* Ensures the image covers the entire background */
  background-position: center;
  /* Centers the image */
  background-repeat: no-repeat;
  /* Prevents the image from repeating */
  overflow: hidden;
}

.transparent-app-bar {
  background: transparent !important;
  /* Fully transparent background */
  box-shadow: none !important;
  /* Remove any shadow */
  display: flex;
  justify-content: flex-start;
  /* Align items to the left */
  padding: 0 20px;
  /* Add some padding for spacing */
}

.transparent-app-bar .rounded-button {
  margin-right: 10px;
  /* Add spacing between buttons */
}

.button-wrapper {
  display: flex;
  justify-content: space-between;
  /* Space between the cards and button */
  align-items: center;
  /* Align items vertically */
  gap: 20px;
  /* Add spacing between the cards and button */
  margin-top: 20px;
  /* Add spacing above the row */
}

.date-card {
  flex: 1;
  /* Make the cards take equal space */
  padding: 2px;
  text-align: center;
  border:2px solid #555;
  border-radius: 10px;
  /* Rounded corners */
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  /* Subtle shadow */
  cursor: pointer;
  /* Make the card clickable */
  transition: box-shadow 0.3s ease;
  background-color: white; /* White background */
  color: black; /* Black text */
}

.date-card:hover {
  box-shadow: 0 6px 8px rgba(0, 0, 0, 0.2);
  /* Elevate on hover */
}

.date-card-title {
  font-size: 1rem;
  font-weight: bold;
  margin: 0;
}


.rounded-btn {
  display: flex; /* Enable flexbox */
  justify-content: center; /* Center the text horizontally */
  align-items: center; /* Center the text vertically */
  background: rgba(255, 255, 255, 0.1); /* Slightly transparent background */
  border: 2px solid black; /* Black border */
  border-radius: 50px; /* Rounded corners */
  padding: 10px 20px; /* Consistent padding for size */
  font-size: 1rem;
  color: black; /* Black text */
  font-weight: bold;
  cursor: pointer;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2); /* Slight elevation */
  transition: background 0.3s ease, color 0.3s ease, box-shadow 0.3s ease;
}

.rounded-btn:hover {
  background: rgba(255, 255, 255, 0.3);
  /* Slightly brighter background on hover */
  color: black;
  /* Change text color on hover */
  box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);
  /* Increase elevation on hover */
}

.rounded-button {
  background: transparent;
  /* Fully transparent background */
  border: 2px solid white;
  /* White border */
  border-radius: 50px;
  /* Rounded corners */
  padding: 5px 30px;
  /* Consistent padding for size */
  font-size: 1rem;
  color: white;
  /* White text */
  font-weight: bold;
  cursor: pointer;
  box-shadow: none;
  /* Remove shadow */
  transition: background 0.3s ease, color 0.3s ease;
  align-self: left;
}

.rounded-button:hover {
  background: rgba(255, 255, 255, 0.2);
  /* Slightly transparent white on hover */
  color: black;
  /* Change text color on hover */
}


.content {
  position: relative;
  z-index: 1;
  /* Ensures the content is on top of the background */
  color: white;
  text-align: center;
  font-size: 2rem;
  padding: 20px;
}

.columns-wrapper {
  display: flex;
  justify-content: space-between;
  /* Space between the two columns */
  align-items: flex-start;
  /* Align columns at the top */
  gap: 20px;
  /* Space between the columns */
  padding: 20px;
  height: calc(100vh - 100px);
  /* Adjust height to fit the page */
}

.column {
  flex: 1;
  /* Make both columns take up equal space */
  background: white;
  /* White background */
  border-radius: 15px;
  /* Rounded corners */
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  /* Subtle shadow */
  padding: 20px;
  overflow: auto;
  /* Handle overflow if content exceeds the column */
}

.column h2 {
  margin-top: 0;
  font-size: 1.5rem;
  color: #333;
  /* Darker text for contrast */
}

.column p {
  font-size: 1rem;
  color: #555;
  /* Slightly lighter text */
}

.v-tabs {
  display: flex;
  justify-content: space-evenly;
  /* Center the tabs horizontally */
  align-items: center;
  /* Align the tabs vertically */
  width: 100%;
}

.v-tab {
  flex: 1;
  /* Make each tab take up equal space */
  text-align: center;
  /* Center the text inside each tab */
  margin: 0;
  /* Add spacing between tabs */
}

.v-tabs-window-item {
  background-color: white;
  /* Set the background color to white */
  border-radius: 10px;
  /* Optional: Add rounded corners */
  padding: 20px;
  /* Optional: Add padding for spacing */
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  /* Optional: Add a subtle shadow */
}

.v-tabs-items {
  background-color: white;
  /* Set the background color for the tab items */
  border-radius: 10px;
  /* Optional: Add rounded corners */
  padding: 20px;
  /* Optional: Add padding for spacing */
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  /* Optional: Add a subtle shadow */
}
</style>