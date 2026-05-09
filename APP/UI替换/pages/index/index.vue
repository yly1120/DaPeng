<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">温度</view>
					<image class="dev-logo" src="../../static/Temp.png" mode=""></image>
				</view>
				<view class="dev-data">{{Temp}} ℃</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">湿度</view>
					<image class="dev-logo" src="../../static/Humi.png" mode=""></image>
				</view>
				<view class="dev-data">{{Humi}} %</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">CO2浓度</view>
					<image class="dev-logo" src="../../static/CO2.png" mode=""></image>
				</view>
				<view class="dev-data">{{CO2_ppm}} ppm</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">光照强度</view>
					<image class="dev-logo" src="../../static/lux.png" mode=""></image>
				</view>
				<view class="dev-data">{{Light}} lux</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">土壤湿度</view>
					<image class="dev-logo" src="../../static/Soil_humi.png" mode=""></image>
				</view>
				<view class="dev-data">{{Soil_Humi}} %</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">土壤PH</view>
					<image class="dev-logo" src="../../static/pH.png" mode=""></image>
				</view>
				<view class="dev-data">{{Soil_ph}} </view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">照明灯</view>
					<image class="dev-logo" src="../../static/LED.png" mode=""></image>
				</view>
				<switch :checked="LED" @change="onLedSwitch" color="#2b9939" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">风扇</view>
					<image class="dev-logo" src="../../static/Fan.png" mode=""></image>
				</view>
				<switch :checked="Fan" @change="onFanSwitch" color="#2b9939" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">蜂鸣器</view>
					<image class="dev-logo" src="../../static/Beep.png" mode=""></image>
				</view>
				<switch :checked="BEEP" @change="onBeepSwitch" color="#2b9939" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">水雾</view>
					<image class="dev-logo" src="../../static/Mist.png" mode=""></image>
				</view>
				<switch :checked="Mist" @change="onMistSwitch" color="#2b9939" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">水泵</view>
					<image class="dev-logo" src="../../static/Pump.png" mode=""></image>
				</view>
				<switch :checked="Pump" @change="onPumpSwitch" color="#2b9939" />
			</view>

			<view class="dev-cart">
				<view class="">
					智能农业大棚
				</view>
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
				LED: false,
				Fan: false,
				BEEP: false,
				Mist: false,
				Pump: false,
				token: '',
				// 可以写阈值 比如:
				// Temp_threshold: 30, // 温度阈值
				switchLed_flag: 0,  // 标记LED开关状态
				switchFan_flag: 0,  // 标记风扇开关状态
				switchBeep_flag: 0, // 标记蜂鸣器开关状态
				switchMist_flag: 0, // 标记雾化器开关状态
				switchPump_flag: 0, // 标记水泵开关状态

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
			// setInterval(() => {
			// 	this.fetchDevData();
			// }, 3000)
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

						if(this.switchLed_flag==1) this.switchLed_flag = 0;
						else this.LED = res.data.data[6].value === 'true';

						if(this.switchFan_flag==1) this.switchFan_flag = 0;
						else this.Fan = res.data.data[3].value === 'true';

						if(this.switchBeep_flag==1) this.switchBeep_flag = 0;
						else this.BEEP = res.data.data[0].value === 'true';

						if(this.switchMist_flag==1) this.switchMist_flag = 0;
						else this.Mist = res.data.data[9].value === 'true';

						if(this.switchPump_flag==1) this.switchPump_flag = 0;
						else this.Pump = res.data.data[10].value === 'true';

						// this.LED = res.data.data[6].value === 'true';
						// this.Fan = res.data.data[3].value === 'true';
						// this.BEEP = res.data.data[0].value === 'true';
						// this.Mist = res.data.data[9].value === 'true';
						// this.Pump = res.data.data[10].value === 'true';


					}
				});
			},
			onLedSwitch(event) {
				console.log(event.detail.value);

				this.switchLed_flag = 1; // 更新LED开关状态标记

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
			},

			onFanSwitch(event) {
				console.log(event.detail.value);

				this.switchFan_flag = 1; // 更新风扇开关状态标记
				
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: {
							"Fan": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('Fan ' + (value ? 'ON' : 'OFF') + ' !');

					}
				});
			},

			onBeepSwitch(event) {
				console.log(event.detail.value);

				this.switchBeep_flag = 1; // 更新蜂鸣器开关状态标记

				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: {
							"BEEP": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('BEEP ' + (value ? 'ON' : 'OFF') + ' !');
					}
				});
			},

			onMistSwitch(event) {
				console.log(event.detail.value);

				this.switchMist_flag = 1; // 更新雾化器开关状态标记
				
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: {
							"Mist": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('Mist ' + (value ? 'ON' : 'OFF') + ' !');
					}
				});
			},

			onPumpSwitch(event) {
				console.log(event.detail.value);

				this.switchPump_flag = 1; // 更新水泵开关状态标记
				
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: {
							"Pump": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('Pump ' + (value ? 'ON' : 'OFF') + ' !');
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

	.dev-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.dev-cart {
		height: 145rpx;
		width: 330rpx;
		border-radius: 30rpx;
		margin-top: 25rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.dev-name {
		font-size: 23rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}

	.dev-data {
		font-size: 35rpx;
		color: #6d6d6d;
	}
</style>