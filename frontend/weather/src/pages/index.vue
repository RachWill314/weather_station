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
      <div class="toggle-switch">
        <label class="switch">
          <input type="checkbox" v-model="isCelsius" @change="toggleTemperatureUnit" />
          <span class="slider"></span>
        </label>
        <p class="toggle-label">{{ isCelsius ? '°C' : '°F' }}</p>
      </div>
      <div class="glossy-container">
        <h2>{{ cardtitle }}</h2>
        <p class="large-number">{{cardsubtitle}}{{ cardunitconvert }}</p>
        <h3 class="subheading">Previous</h3>
        <div class="previous-box">
          <div class="temperature">23°C</div>
          <div class="divider"></div>
          <div class="temperature">24°C</div>
          <div class="divider"></div>
          <div class="temperature">22°C</div>
        </div>
      </div>
      <div class="weather-icons">
        <div class="circle-container" @click="Mqtt.stateChange(0)">
          <div class="circle">
            <img src="../assets/weathersym/temp.svg" alt="Temperature" />
          </div>
          <p class="weather-name">Temperature</p>
          <p class="weather-value">{{payload.temperature}}{{ cardunitconvert }}</p>
        </div>
        <div class="circle-container" @click="Mqtt.stateChange(5)">
          <div class="circle">
            <img src="../assets/weathersym/heat.svg" alt="Heat Index" />
          </div>
          <p class="weather-name">Heat Index</p>
          <p class="weather-value">{{payload.heatindex}}{{ cardunitconvert }}</p>
        </div>
        <div class="circle-container" @click="Mqtt.stateChange(2)">
          <div class="circle">
            <img src="../assets/weathersym/airpressure.svg" alt="Air Pressure" />
          </div>
          <p class="weather-name">Air Pressure</p>
          <p class="weather-value">{{payload.pressure}} hPa</p>
        </div>
        <div class="circle-container" @click="Mqtt.stateChange(3)">
          <div class="circle">
            <img src="../assets/weathersym/altitude.svg" alt="Altitude" />
          </div>
          <p class="weather-name">Altitude</p>
          <p class="weather-value">{{payload.altitude}} m</p>
        </div>
        <div class="circle-container" @click="Mqtt.stateChange(1)">
          <div class="circle">
            <img src="../assets/weathersym/humidity.svg" alt="Humidity" />
          </div>
          <p class="weather-name">Humidity</p>
          <p class="weather-value">{{payload.humidity}}%</p>
        </div>
        <div class="circle-container" @click="Mqtt.stateChange(4)">
          <div class="circle">
            <img src="../assets/weathersym/soil.svg" alt="Soil Moisture" />
          </div>
          <p class="weather-name">Soil Moisture</p>
          <p class="weather-value">{{payload.soil}}%</p>
        </div>
      </div>
      <br>
      <!-- <button class="rounded-button">Live Graph</button> -->
    </div>
  </div>
</template>


<script setup>
// You can add any necessary logic here
// IMPORTS
import { useMqttStore } from "../stores/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";


// import { useAppStore } from "@/store/appStore";
import { ref, reactive, watch, onMounted, onBeforeUnmount, computed, } from "vue";

// VARIABLES
const router = useRouter();
const route = useRoute();
const Mqtt = useMqttStore();

const { payload, payloadTopic, cardtitle, cardsubtitle, cardunit, cardunitconvert } =  storeToRefs(Mqtt);
const host= ref("broker.emqx.io");
const port= ref(9002);
const point= ref(10);
const shift= ref(false);
let isActive = ref(false);

// Method to navigate to the Home page

// Reactive state for toggle switch
const isCelsius = ref(true);

//Method to toggle temperature unit
const toggleTemperatureUnit = () => {
  if (isCelsius.value) {
    cardunitconvert.value = '°C';
    payload.value.temperature = (payload.value.temperature -3) * 5/9;
    payload.value.heatindex = (payload.value.heatindex -32) * 5/9;
  } else {
    payload.value.temperature = (payload.value.temperature * 9/5) + 32;
    payload.value.heatindex = (payload.value.heatindex * 9/5) + 32;
    cardunitconvert.value = '°F';
  }
};

onMounted(() => {
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout(() => {
      // Subscribe to each topic
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
  

</script>

<style scoped>
.image-background {
  position: relative;
  width: 100%;
  height: 100vh; /* Full viewport height */
  background-image: url('../assets/purplesky.jpg'); /* Replace with your image path */
  background-size: cover; /* Ensures the image covers the entire background */
  background-position: center; /* Centers the image */
  background-repeat: no-repeat; /* Prevents the image from repeating */
  overflow: hidden;
}

/* .top-right-buttons {
  position: absolute;
  top: 20px;
  right: 20px;
  display: flex;
  gap: 10px;
} */

.transparent-app-bar {
  background: transparent !important; /* Fully transparent background */
  box-shadow: none !important; /* Remove any shadow */
  display: flex;
  justify-content: flex-start; /* Align items to the left */
  padding: 0 20px; /* Add some padding for spacing */
}

.transparent-app-bar .rounded-button {
  margin-right: 10px; /* Add spacing between buttons */
}

.rounded-button {
  background: transparent; /* Fully transparent background */
  border: 2px solid white; /* White border */
  border-radius: 50px; /* Rounded corners */
  padding: 5px 30px; /* Consistent padding for size */
  font-size: 1rem;
  color: white; /* White text */
  font-weight: bold;
  cursor: pointer;
  box-shadow: none; /* Remove shadow */
  transition: background 0.3s ease, color 0.3s ease;
  align-self: left;
}

.rounded-button:hover {
  background: rgba(255, 255, 255, 0.2); /* Slightly transparent white on hover */
  color: black; /* Change text color on hover */
}

.content {
  position: relative;
  z-index: 1; /* Ensures the content is on top of the background */
  color: white;
  text-align: center;
  font-size: 2rem;
  padding: 20px;
}

.glossy-container {
  width: 300px;
  padding: 20px;
  margin: 20px auto;
  margin-bottom: 70px; /* Additional margin at the bottom */
  background: linear-gradient(145deg, rgb(142, 156, 219), rgb(191, 140, 171)); /* Glossy effect with transparency */
  border: 2px solid rgba(255, 255, 255, 0.5); /* Semi-transparent border */
  border-radius: 50px; /* Rounded corners */
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1), inset 0 -2px 4px rgba(255, 255, 255, 0.6); /* Outer and inner shadows */
  text-align: left; /* Left-align content */
}

.glossy-container h2 {
  margin: 0;
  font-size: 1.5rem;
  color: white; /* Darker text for contrast */
  text-align: center;
}

.glossy-container .large-number {
  margin: 10px 0 0;
  font-size: 5rem;
  font-weight: bold;
  color: whitesmoke; /* Slightly darker text for the number */
  text-align: center;
}

.glossy-container .subheading {
  margin-top: 20px;
  font-size: 1.2rem;
  color: white;
  font-weight: bold;
}

.previous-box {
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: rgba(255, 255, 255, 0.5); /* Slightly transparent background */
  border-radius: 10px;
  padding: 10px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  height: 60px;
}

.temperature {
  font-size: 1rem;
  font-weight: bold;
  color: #333;
  text-align: center;
  flex: 1; /* Equal width for each temperature */
}

.divider {
  width: 1px;
  height: 100%;
  background: rgba(0, 0, 0, 0.2); /* Divider color */
}

.weather-icons {
  display: flex;
  justify-content: center;
  gap: 50px;
  margin-top: 20px;
  /* align-items: center; */
}

.circle {
  width: 50px;
  height: 50px;
  border-radius: 50%;
  background: rgba(255, 255, 255, 0.5); /* Slightly transparent background */
  display: flex;
  justify-content: center;
  align-items: center;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  transition: transform 0.2s ease, background 0.3s ease; /* Smooth transition for hover and click effects */

}

.circle:hover {
  background: rgba(255, 255, 255, 0.8); /* Brighter background on hover */
  transform: scale(1.1); /* Slightly enlarge the circle on hover */
}

.circle:active {
  background: rgba(255, 255, 255, 1); /* Fully opaque background on click */
  transform: scale(0.95); /* Slightly shrink the circle on click */
}

.circle-container {
  display: flex;
  flex-direction: column; /* Stack items vertically */
  align-items: center; /* Center items horizontally */
  text-align: center; /* Center text */
}

.circle img {
  width: 70%;
  height: 70%;
  object-fit: contain;
}

.weather-name {
  margin-top: 5px;
  font-size: 0.9rem;
  color: white;
  font-weight: bold;
}

.weather-value {
  font-size: 0.8rem;
  color: white;
}

.toggle-switch {
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 20px 0;
}

.switch {
  position: relative;
  display: inline-block;
  width: 50px;
  height: 25px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(255, 255, 255, 0.5);
  border-radius: 25px;
  transition: 0.4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 19px;
  width: 19px;
  left: 3px;
  bottom: 3px;
  background-color: white;
  border-radius: 50%;
  transition: 0.4s;
}

input:checked + .slider {
  background-color: rgba(255, 255, 255, 0.8);
}

input:checked + .slider:before {
  transform: translateX(25px);
}

.toggle-label {
  margin-left: 10px;
  font-size: 1rem;
  color: white;
  font-weight: bold;
}
</style>