<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <splashpage :baseURL="baseURL" :warningCode="computedWarning"></splashpage>
                        <mychart :baseURL="baseURL" :time="time" :data="temp" class="white"></mychart>
                        <mychart :baseURL="baseURL" :time="time" :data="tds" class="gray"></mychart>
                        <mychart :baseURL="baseURL" :time="time" :data="tb" class="white"></mychart>
                        <mychart :baseURL="baseURL" :time="time" :data="flow" class="gray"></mychart>
                        <mychart :baseURL="baseURL" :time="time" :data="cdt" class="white"></mychart>
                        <mychart :baseURL="baseURL" :time="time" :data="ph" class="white"></mychart>
                        <params :baseURL="baseURL" :params="computedParams" class="gray"></params>
                        <spoofwarning :baseURL="baseURL" class="white"></spoofwarning>
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
import params from '../components/params.vue';
import spoofwarning from '../components/spoofwarning.vue';

export default {
    name: 'IndexPage',
    components: {
        heady,
        footy,
        mychart,
        splashpage,
        params,
        spoofwarning,
    },
    async fetch() {
        let url = process.env.NODE_ENV === 'development' ? `${process.env.HOST}:${process.env.PORT}` : `${process.env.PROD_URL}`;
        let baseURL = `http://${url}`
        console.log(baseURL);
        this.baseURL = baseURL
        this.testData = await fetch(`${baseURL}/sensorData`).then(res => res.json());
        this.warning = await fetch(`${baseURL}/getWarning`).then(res => res.json());
        this.params = await fetch(`${baseURL}/getParams`).then(res => res.json());
    },
    data() {
        return {
            baseURL: '',
            testData: [],
            warning: 0,
            params: {
                temp: [],
                ph: [],
                flow: [],
                tds: [],
                cdt: [],
                tb: [],
                freq: [],
            },
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
        tds() {
            return {
                name: "TDS",
                data: this.testData.map(function (el) { return el.tds }),
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
        ph() {
            return {
                name: "pH",
                data: this.testData.map(function (el) { return el.ph }),
                yAxisLabel: 'pH'
            }
        },
        computedParams(){
            console.log(this.params);
            return this.params[0]
        },
        computedWarning(){
            console.log(this.warning);
            return this.warning.warningCode;
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
