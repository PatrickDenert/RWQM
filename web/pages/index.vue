<template>
    <div>
        <heady/>
            <div :class="'sum-padding'">
                <div id="chart">
                    <client-only>
                        <mychart :data="temp" :ident="name"></mychart>
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
import testData from '~/data/testdata'; 

export default {
  name: 'IndexPage',
  components: {
    heady, 
    footy,
    mychart,
  },
  
  data(){
        return{
            p: testData,
        }
    },
    computed: {
        temp() {
            return { 
                name: "Temperature",
                color: "Red",
                data: this.p.map(function (el) { return {date: el.time, amount: el.temp}})
            }
        },
        ph() {
            return { 
                name: "PH",
                color: "Blue",
                data: this.p.map(function (el) {  return {date: el.time, amount: el.ph}})
            }
        },
        tb() {
            return { 
                name: "Turbidity",
                color: "Green",
                data: this.p.map(function (el) {  return {date: el.time, amount: el.tb} })
            }
        },
        flow() {
            return { 
                name: "Flow",
                color: "Yellow",
                data: this.p.map(function (el) { return { date: el.time, amount: el.flow} })
            }
        },
        cdt() {
            return { 
                name: "Conductivity",
                color: "Purple",
                data: this.p.map(function (el) { return {date: el.time, amount: el.cdt}})
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
