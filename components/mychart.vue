<template>
    <client-only>
        <div class="flex margin-bottom">
            <div :class="data.name" class="centered"></div>
        </div>
    </client-only>
</template>

<script>
import timeseries from 'timeseries-analysis'

export default {
    name: 'IndexPage',
    props: {
        time: {
            type: Object,
            default: () => {return []}
        },
        data: {
            type: Object,
            default: () => {return {}}
        },
    },
    data() {
        return {
            options: {
                series: [{
                    name: this.data.name,
                    type: 'line',
                    data: this.series,
                },
                // {
                //     name: 'Moving Average',
                //     type: 'line',
                //     data: this.ma,
                // }
            ],
                chart: {
                    stacked: false,
                    height: 350,
                    width: 700,
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
                        text: 'Temperature (F)'
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
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        max() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        avg() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
         },
        ma() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        LinReg() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        laglessDenoise() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        instantTrendline() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        sarimax() {
            var t = new timeseries.main(timeseries.adapter.fromArray(this.data.data));
            var processed = t.ma().output();
            return processed.map((el, index) => {return [this.time.data[index], el[1]]})
        },
        apexcharts() {
            if(process.client) {
                return require('apexcharts')
            }
        }
    },
    methods: {
        onClick() {
            clearTimeout()

            let className = "." + this.data.name
            var chart = new this.apexcharts(document.querySelector(className), this.options);
            chart.render();
        }
    },
    mounted(){
        this.options.series[0].data = this.series

        setTimeout(this.onClick, 1000);
    }
}
</script>

<style>
.centered {
    display: block;
    margin: auto;
    padding: 20px;
    border: 3px solid #F0F8FF;
}

.flex {
    display: flex;
}

.margin-bottom {
    margin-bottom: 200px;
}
</style>
