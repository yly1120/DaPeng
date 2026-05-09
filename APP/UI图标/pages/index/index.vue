<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">温度</view>
					<image class="dev-logo" src="../../static/logo.png" mode=""></image>
				</view>
				<view class="dev-data">{{Temp}} ℃</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">湿度</view>
					<image class="dev-logo" src="../../static/logo.png" mode=""></image>
				</view>
				<view class="dev-data">{{Humi}} %</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">照明灯</view>
					<image class="dev-logo" src="../../static/logo.png" mode=""></image>
				</view>
				<switch :checked="LED" @change="onLedSwitch" color="#8f8f94" />
			</view>

		</view>
	</view>



	<!-- <view class="content">
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
		
	</view> -->
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
				author_key: '6580df399c794cb69cac54dedb9ac822', // 用户key
				version: '2022-05-01', // 版本号
				user_id: '499429', // 用户id
			}

			this.token = createCommonToken(params);

		},
		onShow() {
			this.fetchDevData();
			// setInterval(()=>{
			// 	this.fetchDevData();
			// },3000)
		},
		methods: {
			fetchDevData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //OneNet接口地址。
					method: 'GET',
					data: {
						product_id: 'Kgrb6PkwHq', // 产品id
						device_name: 'System' // 设备名称
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
			onLedSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: {
							"LED": value

						}
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
	.wrap {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.dev-cart {
		height: 150rpx;
		width: 320rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.dev-name {
		font-size: 20rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}

	.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}

	.dev-data {
		font-size: 50rpx;
		color: #6d6d6d;
	}
</style>