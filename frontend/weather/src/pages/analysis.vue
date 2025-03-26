<template>
    <div class="image-background">
      <div class="top-right-buttons">
        <button class="rounded-button">Home</button>
        <button class="rounded-button">Analysis</button>
      </div>
      <div class="content">
        <h3 style="text-align: left;">ELET2415 Weather Station</h3>
      </div>
    </div>
</template>

<script>
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

.top-right-buttons {
  position: absolute;
  top: 20px;
  right: 20px;
  display: flex;
  gap: 10px; /* Space between buttons */
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
</style>
    
    