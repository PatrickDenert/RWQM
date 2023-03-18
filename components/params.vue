<template>
    <div class="update-params">
        <div class="section-title">
            Update Parameters
        </div>
        <div class="row">
            <div class="param-name">
                Parameter name
            </div>
            <div class="min-label">
                Min:
            </div>
            <div class="max-label">
                Max:
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                Temperature
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.temp[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.temp[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                pH
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.ph[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.ph[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                Flow
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.flow[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.flow[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                TDS
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.tds[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.tds[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                Conductivity
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.cdt[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.cdt[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                Turbidity
            </div>
            <div class="min">
                <input type="number" name="" v-model="params.tb[0]">
            </div>
            <div class="max">
                <input type="number" name="" v-model="params.tb[1]">
            </div>
        </div>
        <div class="row">
            <div class="param-name">
                Frequency
            </div>
            <div class="min">
                days:
                <input type="number" name="" v-model="days">
            </div>
            <div class="min">
                hours:
                <input type="number" name="" v-model="hours">
            </div>
            <div class="max">
                15 minute increments:
                <input type="number" name="" v-model="minutes_15">
            </div>
        </div>

        <div class="row button-row">
            <button @click="updateParams" >Update Params</button>
            <button @click="resetToDefaults">Reset to Defaults</button>
        </div>
    </div>
</template>

<script>
export default {
    name: 'params',
    props: {
        params: {
            type: Object,
            default: () => {return {
                temp: [],
                ph: [],
                flow: [],
                tds: [],
                cdt: [],
                tb: [],
                freq: 96,
            }}
        },
        baseURL: {
            type: String,
            default: () => {return ''}
        },
    },
    data() {
        return {
            defaultParams: {
                temp: [0, 40],
                ph: [5, 9],
                flow: [-100, 100],
                tds: [0, 500],
                cdt: [0, 1000],
                tb: [0, 1],
                freq: 96,
            }
        }
    },
    computed: {
        days() {
            return Math.floor(this.params.freq / 96)
        },
        hours() {
            return Math.floor((this.params.freq - 96*this.days) / 4)
        },
        minutes_15() {
            return this.params.freq - 96*this.days - 4*this.hours
        },
    },
    methods: {
        async updateParams() {
            const url = `${this.baseURL}/updateParams`
            const response = await fetch(url, {
                method: "PATCH",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(this.params),
            });
            console.log(response.json());
        },
        resetToDefaults(){
            this.params = this.defaultParams
            this.updateParams();
        }
    },
}
</script>

<style>
.update-params {
        padding: 40px;
        height:70vh;
}

.row {
    display: flex;
    flex-direction: row;
    max-width: 1000px;
    margin: auto;
    padding: 10px;
}

.param-name {
    flex: 1 1 auto;
}

.min-label {
    flex: 0, 0, auto;
    margin-right: 20px;
    width: 154px;
    text-align: center;
}

.max-label {
    flex: 0, 0, auto;
    width: 154px;
    text-align: center;
}

.min {
    flex: 0, 1, auto;
    margin-right: 20px
}

.button-row{
    margin-top: 60px
}

</style>
