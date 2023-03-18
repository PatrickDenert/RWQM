<template>
  <div class="warning">
      {{message}}
      <div v-if="warningCode">
          <button @click="dismissWarnings">Dismiss Warnings</button>
      </div>
  </div>
</template>

<script>
export default {
    name: 'warning',
    props: {
        warningCode:{
            type: Number,
            default: () => {return 0}
        },
        message: {
            type: String,
            default: () => {return ""}
        },
        baseURL: {
            type: String,
            default: () => {return ''}
        },
    },
    methods: {
        async dismissWarnings() {
            console.log('jgf');
            const url = `${this.baseURL}/deleteWarnings`
            const response = await fetch(url, {
                method: "DELETE",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(this.params),
            });
            
            this.warningCode = 0;
            this.message = "There are no warnings at this time"

            console.log(response.json());
        }
    }
}
</script>

<style lang="css">
.warning{
    padding: 100px;
    color: white;
    font-size: 20px;
    text-align: center;
    border-radius: 20px;
    margin:auto;
    max-width: 700px;
    margin-bottom: 200px;
    box-shadow: 10px 5px 5px rgba(0,0,0,0.2);
}
</style>
