<template>
    <div class="spoof-warning">
        <div class="section-title">
            Spoof a Warning
        </div>
        <div class="warning-row">
            <span class="warning-title">
                Warning Code:
            </span>
            <span class="min">
                <input type="number" v-model="warningCode">
            </span>
        </div>
        <div class="row-button">
            <button @click="sendSpoofedWarning">Send Warning</button>
        </div>
    </div>
</template>

<script>
export default {
    name: 'spoofwarning',
    props: {
        baseURL: {
            type: String,
            default: () => {return ''}
        },
    },
    data(){
        return{
            warningCode: 0,
        }
    },
    methods: {
        async sendSpoofedWarning() {
            const url = `${this.baseURL}/warning`
            const response = await fetch(url, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ warningCode: this.warningCode }),
            });
            console.log(response.json());
        }
    }

}
</script>

<style>
.spoof-warning{
    height: 30vh;
    padding: 30px;
}

.warning-row {
text-align: center;
}

.row-button{
    text-align: center;
    margin-top: 30px;
}

.warning-title {
    flex: 0 1 auto;
    margin-right: 20px;
}

.min {
    flex: 0, 1, auto;
}

</style>
