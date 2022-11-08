<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <mychart :data="temp"></mychart>
                        <mychart :data="ph"></mychart>
                        <mychart :data="tb"></mychart>
                        <mychart :data="flow"></mychart>
                        <mychart :data="cdt"></mychart>
                    </client-only>
                </div>
            </div>
        <footy/>
    </div>
</template>

<script>
import heady from '../components/header';
import footy from '../components/footer';
import mychart from '../components/mychart';

export default {
    name: 'IndexPage',
    components: {
        heady, 
        footy,
        mychart,
    },
    async fetch() {
        this.testData = await fetch('https://water-quality-monitor.herokuapp.com/sensorData').then(res => res.json());
    },
    data() {
        return {
            testData: []
        }
    },
    computed: {
        temp() {
            return { 
                name: "Temperature",
                color: "Red",
                data: this.testData.map(function (el) { return [el.time, el.temp]})
            }
        },
        ph() {
            return { 
                name: "PH",
                color: "Blue",
                data: this.testData.map(function (el) {  return [el.time, el.ph]})
            }
        },
        tb() {
            return { 
                name: "Turbidity",
                color: "Green",
                data: this.testData.map(function (el) {  return [el.time, el.tb] })
            }
        },
        flow() {
            return { 
                name: "Flow",
                color: "Yellow",
                data: this.testData.map(function (el) { return [el.time, el.flow] })
            }
        },
        cdt() {
            return { 
                name: "Conductivity",
                color: "Purple",
                data: this.testData.map(function (el) { return [el.time, el.cdt]})
            }
        },
    } 
}
</script>

<style>
.sum-padding {
    padding-top: 100px;
}
</style>
