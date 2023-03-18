<template>
    <div class="splashpage">
        <warning :warningCode = "warningCode"
            :baseURL="baseURL"
            :class="warningColor"
            :message="warningMessage">
        </warning>
    </div>
</template>

<script>
import warning from './warning.vue';

export default {
    name:'splashpage',
    props: {
        warningCode: {
            type: Number,
            default: () => {return 0}
        },
        baseURL: {
            type: String,
            default: () => {return ''}
        },
    },
    components:{
        warning,
    },
    computed: {
        warningMessage(){
            if(!this.warningCode){return "There are no warnings at this time"}
            const warnings = [];
            if (this.warningCode&0x01) {console.log('temperature');warnings.push('temperature')}
            if (this.warningCode&0x02) {console.log('tds');warnings.push('tds')}
            if (this.warningCode&0x04) {console.log('cdt');warnings.push('conductivity')}
            if (this.warningCode&0x08) {console.log('tbd');warnings.push('turbidity')}
            let warningString= warnings.toString(', ');
            console.log(warningString);
            const message = "The Following metrics were measured outside of the acceptable range: " + warningString;
            return message
        },
        warningColor(){
            let color = this.warningCode ? 'red' : 'green';
            return color
        }
    },
}
</script>

<style>
.splashpage{
    background-image: url('../assets/pond.jpeg');
    background-color: #fbfbfb;
    height: 100vh;
    width: 100%;
    padding: 200px;
}
.red{
    background-color: #800000;
}

.green {
    background-color: #3D7A09;
}

button {
    color: white;
    background-color: #800000;
    border: none;
    margin: auto;
    padding: 20px;
    font-size: 20px;
    border-radius: 10px;
}
</style>
