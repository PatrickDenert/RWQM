<template>
    <client-only>
        <div class="mychart">
            <div class="section-title">
                {{data.name}}
            </div>
            <div class="flex">
                <div :class="data.name" class="centered "></div>
            </div>

            <div class="heading">
                Analytics
            </div>

            <div class="mma">
                <div class="mmaspan">Min: {{min}}</div>
                <div class="mmaspan">Max: {{max}}</div>
                <div class="mmaspan">Avg: {{avg}}</div>
            </div>

            <div>
                <ul class="flex-container">
                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('Moving Average')">
                            <span> Moving Average</span>
                        </label>
                        <div class="ma-options" v-if="show['Moving Average']">
                            <div class="slidecontainer">
                                <div>moving average window</div>
                                <input type="range" min="2" max="50" class="slider" v-model="maWindow" v-on:input="updateMA">
                            </div>
                        </div>
                    </li>

                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('Lagless Denoising')">
                            <span> Lagless Denoising</span>
                        </label>
                        <div class="ld-options" v-if="show['Lagless Denoising']">
                            <div class="slidecontainer">
                                <div>Lagless Denoising Window</div>
                                <input type="range" min="2" max="50" class="slider" v-model="smootherPeriod" v-on:input="updateLD">
                            </div>
                        </div>
                    </li>

                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('Instantaneous Trendline')">
                            <span>Instantaneous Trendline</span>
                        </label>
                        <div class="it-options" v-if="show['Instantaneous Trendline']">
                            <div>Alpha Value</div>
                            <input type="range" min="0.01" max="0.7" step="0.01" class="slider" v-model="iTrendAlpha" v-on:input="updateIT">
                        </div>
                    </li>
                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('Linear Regression')">
                            <span>Linear Regression</span>
                        </label>
                    </li>

                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('Windowed Regression')">
                            <span>Windowed Regression</span>
                            <div class="wr-options" v-if="show['Windowed Regression']">
                                <div class="">
                                    <span>Smoothing Window</span>
                                    <input type="range" min="2" max="50" class="slider" v-model="wrSmootherPeriod" v-on:input="updateWR">
                                </div>
                                <div class="">
                                    <span>Sample Size</span>
                                    <input type="range" min="5" max="50" class="slider" v-model="wrSampleSize" v-on:input="updateWR">
                                </div>
                                <div class="">
                                    <span>Degree</span>
                                    <input type="range" min="0" max="10" class="slider" v-model="wrDegree" v-on:input="updateWR">
                                </div>

                                <input type="radio" checked="checked" @click="setARME">
                                <label for="html">ARMaxEntropy</label></br>
                                <input type="radio" @click="setARLS">
                                <label for="css">ARLeastSquare</label>
                            </div>
                        </label>
                    </li>

                    <li class ="flex-element-grow">
                        <label>
                            <input type="checkbox" @click="onRadioButton('SARIMAX Forecasting')">
                            <span>SARIMAX Forecasting</span>
                        </label>
                        <div class="sa-options" v-if="show['SARIMAX Forecasting']">
                            SA options go here
                        </div>
                    </li>
                </ul>
            </div>
        </div>
    </client-only>
</template>

<script>
import timeseries from 'timeseries-analysis'
import ss from 'simple-statistics'
//import ARIMAPromise from 'arima/async'

export default {
    name: 'IndexPage',
    props: {
        time: {
            type: Object,
            default: () => {return []}
        },
        refreshkey: {
            type: Number,
            default: () => {return 0}
        },
        data: {
            type: Object,
            default: () => {return {}}
        },
        baseURL: {
            type: Object,
            default: () => {return {}}
        },
    },
    data() {
        return {
            show: {
                'Moving Average':false,
                'Lagless Denoising':false,
                'Instantaneous Trendline':false,
                "Linear Regression":false,
                'Windowed Regression':false,
                "SARIMAX Forecasting": false,

            },
            sarimax:[],
            maWindow: 20,
            iTrendAlpha: 0.08,
            smootherPeriod: 10,
            wrSmootherPeriod: 10,
            wrSampleSize: 20,
            wrDegree: 4,
            wrMethod: 'ARMaxEntropy',
            options: {
                series: [
                    { name: this.data.name, type: 'line', data: this.series },
                    { name: 'Moving Average', type: 'line', data: [] },
                    { name: 'Lagless Denoising', type: 'line', data: []},
                    { name: 'Instantaneous Trendline', type: 'line', data: [] },
                    { name: 'Linear Regression', type: 'line', data: [] },
                    { name: 'Windowed Regression', type: 'line', data: [] },
                    { name: 'SARIMAX Forecasting', type: 'line', data: [] },
                ],
                colors: ['#2E93fA', '#66DA26', '#546E7A', '#E91E63', '#FF9800', '#7E36AF', '#D9534F'],
                chart: {
                    animations:{
                        enabled: false,
                    },
                    id: this.data.name,
                    stacked: false,
                    height: 500,
                    width: 1000,
                    zoom: {
                        type: 'x',
                        enabled: true,
                        autoScaleYaxis: false
                    },
                    toolbar: {
                        autoSelected: 'zoom'
                    }
                },
                dataLabels: {
                    enabled: false
                },
                markers: {
                    size: 0,
                },
                title: {
                    text: this.data.name,
                    align: 'left'
                },
                yaxis: {
                    labels: {
                        formatter: function (val) {
                            return val.toFixed(1);
                        },
                    },
                    title: {
                        text: this.data.yAxisLabel
                    },
                },
                xaxis: {
                    type: 'datetime',
                },
                tooltip: {
                    shared: false,
                    y: {
                        formatter: function (val) {
                            return (val).toFixed(3)
                        }
                    }
                }
            }
        }
    },
    computed: {
        series() {
            return this.data.data.map((el, index) => {return [this.time.data[index], el]})
        },
        min() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var min = t.min();
            return min.toFixed(2);
        },
        max() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var max = t.max();
            return max.toFixed(2);
        },
        avg() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var avg = t.mean();
            return avg.toFixed(2);
         },
        ma() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma({period: this.maWindow}).output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        laglessDenoise() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.smoother({period:this.smootherPeriod}).output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        instantTrendline() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.dsp_itrend({alpha: this.iTrendAlpha }).output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        LinReg() {
            let trainingData = this.ma.map((el, index) => {return [new Date(el[0]).getTime(), el[1]]});
            let l = ss.linearRegressionLine(ss.linearRegression(trainingData));
            return this.time.data.map((el, i) => {return [el, l(trainingData[i][0])]});
        },
        windowedRegression() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            t.smoother({period:this.wrSmootherPeriod}).save('smoothed');
            var data = t.sliding_regression_forecast({ sample: this.wrSampleSize, degree: this.wrDegree, method: this.wrMethod });
            return data.data.map((el, index) => {return [this.time.data[index], el[1]]});
        },
        apexcharts() {
            if(process.client) {
                return require('apexcharts');
            }
        },
        chart(){
            const className = "." + this.data.name
            const chart = new this.apexcharts(document.querySelector(className), this.options);
            this.charty = chart;
            return chart;
        }
    },
    methods: {
        appendNewData(data){
            this.options.series[0].data.push(data);
            //this.updateAnalytics();
            this.setSeriesObject();
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series)
        },
        timeout() {
            clearTimeout();
            this.options.series[0].data = this.series;
            this.chart.render();
        },
        async onRadioButton(series) {
            if(series === "SARIMAX Forecasting") {
                console.log('calling sarimax');
                let predictions = await this.getSarimaxPredictions();
                console.log(predictions);
                this.sarimax = await this.formatSarmaxPredictions(predictions.pred);
                console.log(this.sarimax);
            }
            this.show[series] =!this.show[series]
            this.setSeriesObject();
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series)
        },
        setSeriesObject(){
            this.options.series[0].data = this.series;
            this.options.series[1].data = this.show['Moving Average'] ? this.ma : [];
            this.options.series[2].data = this.show['Lagless Denoising'] ? this.laglessDenoise : [];
            this.options.series[3].data = this.show['Instantaneous Trendline'] ? this.instantTrendline : [];
            this.options.series[4].data = this.show['Linear Regression'] ? this.LinReg : [];
            this.options.series[5].data = this.show['Windowed Regression'] ? this.windowedRegression : [];
            this.options.series[6].data = this.show['SARIMAX Forecasting'] ? this.sarimax : [];
        },
        updateMA() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma({period: this.maWindow}).output();
            var ma = processed.map((el, index) => {return [this.time.data[index], el[1]]})
            this.options.series[1].data = ma;
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series)
        },
        updateLD() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.smoother({period:this.smootherPeriod}).output();
            var ld = processed.map((el, index) => {return [this.time.data[index], el[1]]})
            this.options.series[2].data = ld;
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series)
        },
        updateWR() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            t.smoother({period:this.wrSmootherPeriod}).save('smoothed');
            var data = t.sliding_regression_forecast({ sample: this.wrSampleSize, degree: this.wrDegree, method: this.wrMethod });
            let wr = data.buffer.map((el, index) => {return [this.time.data[index], el[1]]});
            this.options.series[5].data = wr;
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series)
        },
        updateIT() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.dsp_itrend({alpha: this.iTrendAlpha }).output();
            let it= processed.map((el, index) => {return [this.time.data[index], el[1]]})
            this.options.series[3].data = it;
            this.apexcharts.exec(this.chart.opts.chart.id, 'updateSeries', this.options.series);
        },
        updateSA() {
            return null
        },
        updateAnalytics(){
            this.updateMA();
            this.updateLD();
            this.updateWR();
            this.updateIT();
            this.updateLR();
        },
        setARME(){
            this.wrMethod='ARMaxEntropy'
            this.updateWR()
        },
        setARLS(){
            this.wrMethod='ARLeastSquare'
            this.updateWR()
        },
        async getSarimaxPredictions() {
            console.log(this.data.data);
            const url = `${this.baseURL}/sarimax`
            let res = await fetch(url, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ts: this.data.data}),
            })
            return res.json()
        },
        async formatSarmaxPredictions(predictions) {
            let firstDate = new Date (this.time.data[this.time.data.length - 1]).getTime();
            return predictions.map((el, i) => {return [new Date(firstDate + 900000*i), el]});
        }
    },
    mounted(){
        setTimeout(this.timeout, 50);
    }
}
</script>

<style>
.centered {
    display: block;
    margin: auto;
    padding: 20px;
    border: 3px solid #F0F8FF;
    background-color: white;
}

.flex {
    display: flex;
}

.mychart {
    padding: 40px;
    height:100vh;
}
.mma {
    padding: 30px;
    display: flex;
    flex-direction: row;
    max-width: 1000px;
    margin: auto;
}

.mmaspan {
flex: 1 1 auto;
text-align: center;
font-weight: bold;
}

.flex-container {
    display: flex;
    flex-direction: row;
}

.flex-element {
    padding: 50px;
    flex: 0 1 auto;
}

.flex-element-grow {
    width: 306px;
    padding: 0px 50px;
    flex: 0 0 auto;
}

.heading {
    padding-top:20px;
    text-align: center;
    font-size: 30px;
}

.section-title {
    color: #000000;
    font-size: 50px;
    font-weight: bold;
    text-align: center;
    margin-bottom: 40px !important;
}


</style>
