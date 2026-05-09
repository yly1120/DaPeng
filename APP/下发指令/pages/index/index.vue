<template>
	<view class="content">
		<view class="">温度 {{Temp}} ℃</view>
		<view class="">湿度 {{Humi}} %</view>
		<view class="">二氧化碳浓度 {{CO2_ppm}} ppm</view>
		<view class="">光照强度 {{Light}} lux</view>
		<view class="">土壤湿度 {{Soil_Humi}} %</view>
		<view class="">土壤PH值 {{Soil_ph}} </view>

		<switch :checked="LED" @change="onLedSwitch" />	 LED
		<switch :checked="Fan" @change="" />	 风扇
		<switch :checked="BEEP" @change="" />	 蜂鸣器
		<switch :checked="Mist" @change="" />	 雾化器
		<switch :checked="Pump" @change="" />	 水泵
		
		
	</view>
</template>

<script>
	const {
		createCommonToken
	} = require('@/key.js')
	export default {
		data() {
			return {
				Temp: '',
				Humi: '',
				CO2_ppm: '',
				Light: '',
				Soil_Humi: '',
				Soil_ph: '',
				LED: true,
				Fan: false,
				BEEP: false,
				Mist: false,
				Pump: false,
				token: '',
			}
		},
		onLoad() {
			const params = {
				author_key: '6580df399c794cb69cac54dedb9ac822',		// 用户key
				version: '2022-05-01',		// 版本号
				user_id: '499429',		// 用户id
			}

			this.token = createCommonToken(params);
			console.log(this.token)

		},
		onShow() {
			this.fetchDevData();
			// setInterval(()=>{
			// 	this.fetchDevData();
			// },3000)
		},
		methods: {
			fetchDevData(){
				uni.request({
				    url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //仅为示例，并非真实接口地址。
					method: 'GET',
				    data: {
						product_id: 'Kgrb6PkwHq',	// 产品id
						device_name: 'System'		// 设备名称
				    },
				    header: {
				        'authorization': this.token //自定义请求头信息
				    },
				    success: (res) => {
				        console.log(res.data);
						this.Temp = res.data.data[15].value;
						this.Humi = res.data.data[4].value;
						this.CO2_ppm = res.data.data[1].value;
						this.Light = res.data.data[7].value;
						this.Soil_Humi = res.data.data[11].value;
						this.Soil_ph = res.data.data[13].value;

						this.LED = res.data.data[6].value === 'true';
						this.Fan = res.data.data[3].value === 'true';
						this.BEEP = res.data.data[0].value === 'true';
						this.Mist = res.data.data[9].value === 'true';
						this.Pump = res.data.data[10].value === 'true';
				    }
				});
			},
			onLedSwitch(event){
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
				    url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
				    data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: { "LED": value }
				    },
				    header: {
				        'authorization': this.token //自定义请求头信息
				    },
				    success: () => {
				        console.log('LED ' + (value ? 'ON' : 'OFF') + ' !');
				    }
				});
			}


		}
	}
</script>

<style>
	.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}

	.logo {
		height: 200rpx;
		width: 200rpx;
		margin-top: 200rpx;
		margin-left: auto;
		margin-right: auto;
		margin-bottom: 50rpx;
	}

	.text-area {
		display: flex;
		justify-content: center;
	}

	.title {
		font-size: 36rpx;
		color: #8f8f94;
	}
</style>
