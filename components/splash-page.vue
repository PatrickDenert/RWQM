<template>
    <div class="splashpage">
        <warning :warningCode = "warningCode"
            :baseURL="baseURL"
            :class="warningColor"
            :message="warningMessage"
            :func="dismissWarnings">
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
    computed: {
        warningMessage(){
            console.log(this.warning)
            if(!this.warningCode){return "There are no warnings at this time"}
            const warnings = [];
            if (this.warningCode & 0x01) {warnings.push('conductivity')}
            if (this.warningCode & 0x02) {warnings.push('tds')}
            if (this.warningCode & 0x04) {warnings.push('temperature')}
            if (this.warningCode & 0x08) {warnings.push('turbidity')}
            if (this.warningCode & 0x10) {warnings.push('battery Level')}
            let warningString= warnings.toString(`, `);
            console.log(warnings.toString(', '));
            console.log(warnings.join(', '));
            console.log(warningString);
            const message = "The Following metrics were measured outside of the acceptable range: " + warningString;
            return message
        },
        warningColor(){
            let color = this.warningCode ? 'red' : 'green';
            return color;
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
