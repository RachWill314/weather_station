import {defineStore} from 'pinia'
import {ref} from 'vue'


export const useAppStore =  defineStore('app', ()=>{
    const temp = ref([])
    const hum = ref([])
    const heat = ref([])
    const timestamp = ref([])

    async function getAllData(start,end){
        try {
            let url = `/app/${Number(start)/1000}/${Number(end)/1000}`
                console.log(url)
              const response = await fetch(url, {
              });
              if (!response.ok) {
                throw new Error(`Error fetching data: ${response.status}`);
              }
            const data = await response.json(); 
            const data1 = data['data']
            console.log(data1)
            hum.value = []
            temp.value = []
            heat.value = []
            timestamp.value = []
            for(let i=0;i<data1.length;i++){
                temp.value = [...temp.value , data1[i].temperature]
                hum.value = [...hum.value, data1[i].humidity]
                heat.value= [...heat.value , data1[i].heatindex]
                const date = new Date(data1[i].timestamp * 1000); // Multiply by 1000 if timestamp is in seconds
                const formattedTime = date.toLocaleString('en-US', { 
                    month: '2-digit',
                    day: '2-digit',
                    hour: '2-digit',
                    minute: '2-digit',
                    hour12: false
                });
                timestamp.value = [...timestamp.value, formattedTime]
            }

            console.log(`the data is ${hum.value}`)
          
            } catch (error) {
                console.log("Error:")
              console.error("Error:", error);
            }
          }
    return { 
    // EXPORTS	
    getAllData,
    temp,
    hum,
    heat,
    timestamp
       }
},{ persist: true  });