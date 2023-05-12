<template>
  <div class="warning">
      {{message}}
      <div v-if="warningCode">
          <button @click="func">Dismiss Warnings</button>
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
        func: {
            type: Function,
            default: () => {return () => {}}
        },
    },
}
</script>

<style lang="css">
.warning{
    padding: 50px;
    color: white;
    font-size: 20px;
    text-align: center;
    border-radius: 20px;
    margin:auto;
    max-width: 700px;
    margin-bottom: 200px;
    box-shadow: 10px 5px 5px rgba(0,0,0,0.2);
}

button {
    margin-top: 50px !important;
    color: black !important;
    background-color: white !important;
}
</style>
