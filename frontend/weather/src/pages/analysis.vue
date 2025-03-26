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
                  <LineGraph :labels="timeLabels" :datasets="temperatureAndHeatIndexData" />
                </v-tabs-window-item>

                <v-tabs-window-item value="Humidity">
                  <LineGraph :labels="timeLabels" :datasets="humidityAndSoilMoistureData" />
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
            <!-- Start Date Button -->
            <v-btn class="rounded-btn" @click="startDateDialog = true">
              Select Start Date
            </v-btn>
            <v-dialog v-model="startDateDialog" persistent max-width="290">
              <v-card>
                <v-date-picker v-model="startDate" @input="startDateDialog = false" />
              </v-card>
            </v-dialog>

            <!-- End Date Button -->
            <v-btn class="rounded-btn" @click="endDateDialog = true">
              Select End Date
            </v-btn>
            <v-dialog v-model="endDateDialog" persistent max-width="290">
              <v-card>
                <v-date-picker v-model="endDate" @input="endDateDialog = false" />
              </v-card>
            </v-dialog>
          </div>


        </div>
      </div>
    </div>
  </div>
</template>

<script>
import LineGraph from "@/components/LineGraph.vue";

export default {
  components: {
    LineGraph,
  },
  data() {
    return {
      tab: "Temperature",
      timeLabels: ["10:00", "11:00", "12:00", "13:00", "14:00"], // Example time labels
      temperatureAndHeatIndexData: [
        {
          label: "Temperature (°C)",
          data: [22, 24, 23, 25, 26], // Example temperature data
          borderColor: "rgba(75, 192, 192, 1)",
          backgroundColor: "rgba(75, 192, 192, 0.2)",
          borderWidth: 2,
          tension: 0.4, // Smooth curve
        },
        {
          label: "Heat Index (°C)",
          data: [23, 25, 24, 26, 27], // Example heat index data
          borderColor: "rgba(255, 99, 132, 1)",
          backgroundColor: "rgba(255, 99, 132, 0.2)",
          borderWidth: 2,
          tension: 0.4, // Smooth curve
        },
      ],

      humidityAndSoilMoistureData: [
        {
          label: "Humidity (%)",
          data: [60, 65, 63, 68, 70], // Example humidity data
          borderColor: "rgba(54, 162, 235, 1)",
          backgroundColor: "rgba(54, 162, 235, 0.2)",
          borderWidth: 2,
          tension: 0.4, // Smooth curve
        },
        {
          label: "Soil Moisture (%)",
          data: [40, 42, 41, 43, 45], // Example soil moisture data
          borderColor: "rgba(153, 102, 255, 1)",
          backgroundColor: "rgba(153, 102, 255, 0.2)",
          borderWidth: 2,
          tension: 0.4, // Smooth curve
        },
      ],

    };
  },
};

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
  justify-content: space-evenly; /* Space between the buttons */
  gap: 20px; /* Space between the buttons */
  margin-top: 20px; /* Add spacing above the buttons */
}

.rounded-btn {
  background: rgba(255, 255, 255, 0.1); /* Slightly transparent background */
  border: 2px solid black; /* White border */
  border-radius: 50px; /* Rounded corners */
  padding: 10px 10px; /* Consistent padding for size */
  font-size: 1rem;
  color: black; /* White text */
  font-weight: bold;
  cursor: pointer;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2); /* Slight elevation */
  transition: background 0.3s ease, color 0.3s ease, box-shadow 0.3s ease;
}

.rounded-btn:hover {
  background: rgba(255, 255, 255, 0.3); /* Slightly brighter background on hover */
  color: black; /* Change text color on hover */
  box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3); /* Increase elevation on hover */
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