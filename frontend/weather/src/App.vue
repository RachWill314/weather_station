<template>
  <v-app>
    <router-view />
  </v-app>
</template>

<script setup>
  //
  setInterval(() => {
  location.reload();
}, 30000);
</script>
