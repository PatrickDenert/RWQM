<template>
    <client-only>
        <div class="flex margin-bottom">
            <svg :class="data.name"></svg>
            <button @click="onClick()">click me</button>
        </div>
    </client-only>
</template>

<script>
import * as d3 from 'd3'

export default {
    name: 'IndexPage',
    props: {
        data: {
            type: Object,
            default: () => {return {}} 
        }
    },
    methods: {
        onClick() {
            const width = 1000;
            const height = 400;

            const data = this.data.data
            console.log(data)

            let id = "." + this.data.name
            const svg = d3.select(id).attr("width", width).attr("height", height);
            const g = svg.append("g");

            //2. Parse the dates
            const parseTime = d3.utcParse("%Y-%m-%dT%H:%M:%S.%LZ");

            //3. Creating the Chart Axes
            const x = d3
            .scaleTime()
            .domain(
                d3.extent(data, function (d) {
                return parseTime(d.date);
                })
            )
            .rangeRound([0, width]);

            const y = d3
            .scaleLinear()
            .domain(
                d3.extent(data, function (d) {
                return d.amount;
                })
            )
            .rangeRound([height, 0]);

            //4. Creating a Line
            const line = d3
            .line()
            .x(function (d) {
                return x(parseTime(d.date));
            })
            .y(function (d) {
                return y(d.amount);
            });

            //5. Appending the Axes to the Chart
            g.append("g")
            .attr("transform", "translate(0," + height + ")")
            .call(d3.axisBottom(x));

            g.append("g")
            .call(d3.axisLeft(y))
            .append("text")
            .attr("fill", "#000")
            .attr("transform", "rotate(-90)")
            .attr("y", 6)
            .attr("dy", "0.71em")
            .attr("text-anchor", "end")
            .text("Price ($)");    

            //6. Appending a path to the Chart
            g.append("path")
            .datum(data)
            .attr("fill", "none")
            .attr("stroke", this.data.color)
            .attr("stroke-width", 1.5)
            .attr("d", line);

        }
    },
}
</script>

<style>
svg {
    display: block;
    margin: auto;
    border: 3px solid #F0F8FF;
}

.flex {
    display: flex;
}

.margin-bottom {
    margin-bottom: 50px;
}
</style>
