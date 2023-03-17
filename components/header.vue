<template>
    <b-navbar class="shadow">
        <template #brand>
            <b-navbar-item tag="router-link" :to="{ path: '/' }">
                <span class='title'>RWQM</span>
            </b-navbar-item>
        </template>
        <template #start>
            <b-navbar-item href="#">
                Home
            </b-navbar-item>
            <b-navbar-item href="#">
                Temp
            </b-navbar-item>
            <b-navbar-item href="#">
                Ph
            </b-navbar-item>
            <b-navbar-item href="#">
                Conductivity
            </b-navbar-item>
            <b-navbar-item href="#">
                Turbidity
            </b-navbar-item>
            <b-navbar-item href="#">
                Flow
            </b-navbar-item>
        </template>

        <template #end>
            <b-navbar-item tag="div">
                <div class="buttons">
                    <a class="button is-primary">
                        <strong>Sign up</strong>
                    </a>
                    <a class="button is-light">
                        Log in
                    </a>
                </div>
            </b-navbar-item>
        </template>
    </b-navbar>
</template>

<script>
export default {
  name: 'heady',
}
</script>

<style>
.shadow{
    box-shadow: 10px 5px 5px rgba(0,0,0,0.2);
}
.title{
    color: #800000;
}

.is-primary {
    background-color: #800000 !important;
}

.is-light {
    background-color: #BEA6A0 !important;
    color:#FFFFFF !important;
}
</style>
