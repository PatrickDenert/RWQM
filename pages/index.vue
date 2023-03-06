<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <mychart :time="time" :data="temp"></mychart>
                        <mychart :time="time" :data="ph"></mychart>
                        <mychart :time="time" :data="tb"></mychart>
                        <mychart :time="time" :data="flow"></mychart>
                        <mychart :time="time" :data="cdt"></mychart>
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
        this.testData = await fetch('http://rwqm.fly.dev/sensorData').then(res => res.json());
    },
    data() {
        return {
            testData: []
        }
    },
    computed: {
        time() {
            return {
                name: "Temperature",
                color: "Red",
                data: this.testData.map(function (el) { return el.time })
            }
        },
        temp() {
            return {
                name: "Temperature",
                color: "Red",
                data: this.testData.map(function (el) { return el.temp })
            }
        },
        ph() {
            return {
                name: "PH",
                color: "Blue",
                data: this.testData.map(function (el) { return el.ph })
            }
        },
        tb() {
            return {
                name: "Turbidity",
                color: "Green",
                data: this.testData.map(function (el) {  return el.tb })
            }
        },
        flow() {
            return {
                name: "Flow",
                color: "Yellow",
                data: this.testData.map(function (el) { return el.flow })
            }
        },
        cdt() {
            return {
                name: "Conductivity",
                color: "Purple",
                data: this.testData.map(function (el) { return el.cdt })
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
