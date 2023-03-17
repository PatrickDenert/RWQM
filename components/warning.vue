<template>
  <div class="warning">
      {{message}}
  </div>
</template>

<script>
export default {
    name: 'warning',
    props: {
        message: {
            type: String,
            default: () => {return ""}
        },
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
    max-width: 500px;
    margin-bottom: 200px;
    box-shadow: 10px 5px 5px rgba(0,0,0,0.2);
}
</style>
