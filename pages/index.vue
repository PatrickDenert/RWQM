<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <splashpage :baseURL="sarimaxURL" :warningCode="warning" :socket="socket" :key="refreshkey" ></splashpage>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="temp" ref="tempChart" class="white" ></mychart>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="tds" ref="tdsChart" class="gray" ></mychart>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="tb" ref="tbChart" class="white" ></mychart>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="flow" ref="flowChart" class="gray" ></mychart>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="cdt" ref="cdtChart" class="white" ></mychart>
                        <mychart :baseURL="sarimaxURL" :time="time" :data="ph" ref="phChart" class="white" ></mychart>
                        <params :baseURL="sarimaxURL" :params="computedParams" class="gray"></params>
                        <spoofwarning :baseURL="sarimaxURL" class="white"></spoofwarning>
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
import { io } from 'socket.io-client'

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
        let sarimaxURL = `https://${url}`
        console.log(baseURL);
        this.baseURL = baseURL
        this.testData = await fetch(`${baseURL}/sensorData`).then(res => res.json());
        this.warning = await fetch(`${baseURL}/getWarning`).then(res => res.json());
        console.log(this.warning);
        this.warning = this.warning.warningCode;
        console.log(this.warning);
        this.params = await fetch(`${baseURL}/getParams`).then(res => res.json());
    },
    data() {
        return {
            newReading: null,
            refreshkey: 0,
            socket: {},
            baseURL: '',
            sarimaxURL: '',
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
    mounted() {
        console.log(this.baseURL);
        const socket = io(this.sarimaxURL);

        socket.on("newReading", (reading) => {
            console.log("newReading", reading);
            this.testData.push(reading);
            this.newReading = reading;
        });

        socket.on("newWarning", (warning) => {
            console.log("newWarning", warning);
            console.log(this.warning);
            this.warning |= warning.warningCode;
            console.log(this.warning);
            this.refreshkey++;
        });

        setInterval(() => {
            console.log("setTimeout");
            if (this.newReading !== null) {
                console.log("new reading exists", this.newReading);
                let temp = this.$refs.tempChart;
                let tds = this.$refs.tdsChart;
                let td = this.$refs.tbChart;
                let flow = this.$refs.flowChart;
                let cdt = this.$refs.cdtChart;
                let ph = this.$refs.phChart;

                let tempData = [this.newReading.time, this.newReading.temp]
                let tdsData = [this.newReading.time, this.newReading.tds]
                let tbData = [this.newReading.time, this.newReading.tb]
                let flowData = [this.newReading.time, this.newReading.flow]
                let cdtData = [this.newReading.time, this.newReading.cdt]
                let phData = [this.newReading.time, this.newReading.ph]

                temp.appendNewData(tempData);
                tds.appendNewData(tdsData);
                td.appendNewData(tbData);
                flow.appendNewData(flowData);
                cdt.appendNewData(cdtData);
                ph.appendNewData(phData);

                this.newReading = null;

                console.log(warning);
            }
        }, 5000);
        this.socket = socket;
    },
    computed: {
        time() {
            this.refreshkey;
            return {
                name: "Time",
                data: this.testData.map(function (el) { return el.time })
            }
        },
        temp() {
            this.refreshkey;
            return {
                name: "Temperature",
                data: this.testData.map(function (el) { return el.temp }),
                yAxisLabel: 'Temperature (C)'
            }
        },
        tds() {
            this.refreshkey;
            return {
                name: "TDS",
                data: this.testData.map(function (el) { return el.tds }),
                yAxisLabel: 'TDS (mg/L)'
            }
        },
        tb() {
            this.refreshkey;
            return {
                name: "Turbidity",
                data: this.testData.map(function (el) {  return el.tb }),
                yAxisLabel: 'Turbidity (NTU)'
            }
        },
        flow() {
            this.refreshkey;
            return {
                name: "Flow",
                data: this.testData.map(function (el) { return el.flow }),
                yAxisLabel: 'Flow (cfs)'
            }
        },
        cdt() {
            this.refreshkey;
            return {
                name: "Conductivity",
                data: this.testData.map(function (el) { return el.cdt }),
                yAxisLabel: 'Conductivity (uS/cm)'
            }
        },
        ph() {
            this.refreshkey;
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
        socket(){
            console.log(this.baseURL);

        }
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
