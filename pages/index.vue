<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <splashpage :warningCode="warning"></splashpage>
                        <mychart :time="time" :data="temp" class="white"></mychart>
                        <mychart :time="time" :data="ph" class="gray"></mychart>
                        <mychart :time="time" :data="tb" class="white"></mychart>
                        <mychart :time="time" :data="flow" class="gray"></mychart>
                        <mychart :time="time" :data="cdt" class="white"></mychart>
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
import splashpage from '../components/splash-page.vue';

export default {
    name: 'IndexPage',
    components: {
        heady,
        footy,
        mychart,
        splashpage,
    },
    async fetch() {
        this.testData = await fetch('http://localhost:3000/sensorData').then(res => res.json());
        this. waringCode = await fetch('http://localhost:3000/getWarning').then(res => res.json()).warningCode;
        this.params = await fetch('http://localhost:3000/getParams').then(res => res.json());
    },
    data() {
        return {
            testData: [],
            warning: 0,
            params: {},
        }
    },
    computed: {
        time() {
            return {
                name: "Time",
                data: this.testData.map(function (el) { return el.time })
            }
        },
        temp() {
            return {
                name: "Temperature",
                data: this.testData.map(function (el) { return el.temp }),
                yAxisLabel: 'Temperature (C)'
            }
        },
        ph() {
            return {
                name: "TDS",
                data: this.testData.map(function (el) { return el.ph }),
                yAxisLabel: 'TDS (mg/L)'
            }
        },
        tb() {
            return {
                name: "Turbidity",
                data: this.testData.map(function (el) {  return el.tb }),
                yAxisLabel: 'Turbidity (NTU)'
            }
        },
        flow() {
            return {
                name: "Flow",
                data: this.testData.map(function (el) { return el.flow }),
                yAxisLabel: 'Flow (cfs)'
            }
        },
        cdt() {
            return {
                name: "Conductivity",
                data: this.testData.map(function (el) { return el.cdt }),
                yAxisLabel: 'Conductivity (uS/cm)'
            }
        },
    }
}
</script>

<style>
.white{
    background-color: white;
}

.gray{
    background-color: #FBFBFB;
}
</style>
