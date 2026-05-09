<template>
	<view class="wrap">
		<!-- 设备区域 -->
		<view class="dev-area">
			<!-- 设备卡片 -->
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">温度</view>
					<!-- 温度图标 -->
					<image class="dev-logo" src="../../static/Temp.png" mode=""></image>
				</view>
				<!-- 温度数据 -->
				<view class="dev-data">{{Temp}} ℃</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">湿度</view>
					<!-- 湿度图标 -->
					<image class="dev-logo" src="../../static/Humi.png" mode=""></image>
				</view>
				<!-- 湿度数据 -->
				<view class="dev-data">{{Humi}} %</view>
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">CO2浓度</view>
					<!-- CO2图标 -->
					<image class="dev-logo" src="../../static/CO2.png" mode=""></image>
				</view>
				<!-- CO2数据 -->
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
					<!-- 光照强度图标 -->
					<image class="dev-logo" src="../../static/Soil_humi.png" mode=""></image>
				</view>
				<!-- 光照强度数据 -->
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

			<!-- 温度阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">温度阈值</view>
					<!-- 当前温度阈值显示 -->
					<view class="dev-name">{{Temp_th}}℃</view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="Temp_th" @change="sliderChange($event, 'slider1')" min="0" max="60" step="1"
						block-size="20" show-value />
				</view>
			</view>

			<!-- 湿度阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">湿度阈值</view>
					<!-- 当前湿度阈值显示 -->
					<view class="dev-name">{{Humi_th}}%</view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="Humi_th" @change="sliderChange($event, 'slider2')" min="0" max="100" step="1"
						block-size="20" show-value />
				</view>
			</view>

			<!-- CO2阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">CO2阈值</view>
					<!-- 当前CO2阈值显示 -->
					<view class="dev-name">{{CO2_ppm_th}} ppm</view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="CO2_ppm_th" @change="sliderChange($event, 'slider3')" min="0" max="2000" step="1"
						block-size="20" show-value />
				</view>
			</view>


			<!-- 光照阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">光照阈值</view>
					<!-- 当前光照阈值显示 -->
					<view class="dev-name">{{Light_th}} lux</view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="Light_th" @change="sliderChange($event, 'slider4')" min="0" max="999" step="1"
						block-size="20" show-value />
				</view>
			</view>

			<!-- 土壤湿度阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">土壤湿度阈值</view>
					<!-- 当前土壤湿度阈值显示 -->
					<view class="dev-name">{{Soil_Humi_th}} %</view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="Soil_Humi_th" @change="sliderChange($event, 'slider5')" min="0" max="100" step="1"
						block-size="20" show-value />
				</view>
			</view>

			<!-- 土壤PH上限阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">土壤PH上限阈值</view>
					<!-- 当前土壤PH上限阈值显示 -->
					<view class="dev-name">{{(Soil_ph_high_x100 / 100).toFixed(2)}} </view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="Soil_ph_high_x100" @change="sliderChange($event, 'slider6')" min="700" max="1400" step="1"
						block-size="20" show-value />
				</view>
			</view>

			<!-- 土壤PH下限阈值长卡片 -->
			<view class="device-cart-l">
				<view>
					<view class="dev-name">土壤PH下限阈值</view>
					<!-- 当前土壤PH下限阈值显示 -->
					<view class="dev-name">{{(x100_soil_ph_low / 100).toFixed(2)}} </view>
				</view>
				<view class="ctrl-slider">
					<!-- 滑动条调用slider组件，实时更新阈值 -->
					<slider :value="x100_soil_ph_low" @change="sliderChange($event, 'slider7')" min="0" max="700" step="1"
						block-size="20" show-value />
				</view>
			</view>



		</view>
	</view>

</template>

<script>
	// 引入字符串处理函数
	import {
		stringify
	} from 'querystring';
	// 引入创建令牌函数
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
				CO2_ppm_th: 1000, 			// CO2阈值
				Humi_th: 25, 				// 湿度阈值
				Light_th: 800, 				// 光照阈值
				Soil_Humi_th: 5, 			// 土壤湿度阈值
				Temp_th: 35, 				// 温度阈值
				Soil_ph_high_x100: 1200, 	// 土壤PH上限
				x100_soil_ph_low: 200, 		// 土壤PH下限

				switchLed_flag: 0, 			// 标记LED开关状态
				switchFan_flag: 0, 			// 标记风扇开关状态
				switchBeep_flag: 0, 		// 标记蜂鸣器开关状态
				switchMist_flag: 0, 		// 标记雾化器开关状态
				switchPump_flag: 0, 		// 标记水泵开关状态
				
				

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
			// 获取设备数据
			this.fetchDevData();
			// 3s获取一次
			setInterval(() => {
				this.fetchDevData();
			}, 3000)
		},
		methods: {
			fetchDevData() {
				// 发送请求获取设备属性
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
						// 成功后更新相关数据变化
						console.log(res.data);
						// 设备数据
						this.Temp = res.data.data[15].value;
						this.Humi = res.data.data[4].value;
						this.CO2_ppm = res.data.data[1].value;
						this.Light = res.data.data[7].value;
						this.Soil_Humi = res.data.data[11].value;
						this.Soil_ph = res.data.data[13].value;
						// 阈值
						this.CO2_ppm_th = res.data.data[2].value;
						this.Humi_th = res.data.data[5].value;
						this.Light_th = res.data.data[8].value;
						this.Soil_Humi_th = res.data.data[12].value;
						this.Soil_ph_high_x100 = res.data.data[14].value;
						this.x100_soil_ph_low = res.data.data[17].value;
						this.Temp_th = res.data.data[16].value;
						// 外设
						if (this.switchLed_flag == 1) this.switchLed_flag = 0;
						else this.LED = res.data.data[6].value === 'true';

						if (this.switchFan_flag == 1) this.switchFan_flag = 0;
						else this.Fan = res.data.data[3].value === 'true';

						if (this.switchBeep_flag == 1) this.switchBeep_flag = 0;
						else this.BEEP = res.data.data[0].value === 'true';

						if (this.switchMist_flag == 1) this.switchMist_flag = 0;
						else this.Mist = res.data.data[9].value === 'true';

						if (this.switchPump_flag == 1) this.switchPump_flag = 0;
						else this.Pump = res.data.data[10].value === 'true';
					}
				});
			},
			// 开关LED
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
			// 开关风扇
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
			// 开关蜂鸣器
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
			// 开关水雾
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
			// 开关水泵
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
			},
			
			// 滑动条变化事件的方法
			sliderChange(e, id) {
				console.log(id)
				console.log('value 发生变化：' + e.detail.value) // 控制台打印信息，调试用
				let key_th = {}
				
				if (id == 'slider1') {
					this.Temp_th = e.detail.value
					// 更新温度阈值
					this.key_th = {
						Temp_th: this.Temp_th,
					};
				} else if (id == 'slider2') {
					this.Humi_th = e.detail.value
					// 更新湿度阈值
					this.key_th = {
						Humi_th: this.Humi_th,
					};
				} else if (id == 'slider3') {
					this.CO2_ppm_th = e.detail.value
					this.key_th = {
						CO2_ppm_th: this.CO2_ppm_th,
					};
				} else if (id == 'slider4') {
					this.Light_th = e.detail.value
					this.key_th = {
						Light_th: this.Light_th,
					};
				} else if (id == 'slider5') {
					this.Soil_Humi_th = e.detail.value
					this.key_th = {
						Soil_Humi_th: this.Soil_Humi_th,
					};
				} else if (id == 'slider6') {
					this.Soil_ph_high_x100 = e.detail.value
					this.key_th = {
						Soil_ph_high_x100: this.Soil_ph_high_x100,
					};
				} else if (id == 'slider7') {
					this.x100_soil_ph_low = e.detail.value
					this.key_th = {
						x100_soil_ph_low: this.x100_soil_ph_low,
					};
				} 
				
				console.log('value 发生变化：>>>>>' + stringify(this.key_th) + '<<<<<<<')
				// 向后端发送设备属性更新请求
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', // OneNet接口地址。
					method: 'POST',
					data: {
						product_id: 'Kgrb6PkwHq',
						device_name: 'System',
						params: this.key_th
					},
					header: {
						'authorization': this.token // 自定义请求头信息
					},
					// 请求成功，打印提示信息，主要用于调试，可自定义
					success: () => {
						console.log(this.key_th);
					}
				});
			}


		}
	}
</script>

<style>
	/* 整体页面容器样式 */
	.wrap {
		padding: 30rpx;
		/* 设置内边距为30像素 */
	}

	/* 设备区域样式 */
	.dev-area {
		display: flex;
		/* 使用弹性盒子布局 */
		justify-content: space-between;
		/* 在弹性容器中均匀分布子元素，两端对齐 */
		flex-wrap: wrap;
		/* 如果子元素溢出容器，则折叠到下一行 */
	}

	/* 设备卡片样式 */
	.dev-cart {
		height: 140rpx;
		/* 设置高度为140像素 */
		width: 320rpx;
		/* 设置宽度为320像素 */
		border-radius: 40rpx;
		/* 设置边框圆角为40像素 */
		margin-top: 30rpx;
		/* 设置上外边距为30像素 */
		display: flex;
		/* 使用弹性盒子布局 */
		justify-content: space-around;
		/* 在弹性容器中均匀分布子元素，两端对齐 */
		align-items: center;
		/* 在弹性容器中垂直居中对齐子元素 */
		box-shadow: 0 0 15rpx #ccc;
		/* 设置盒子阴影，颜色为灰色 */
	}
	
	/* 长设备卡片样式 */
	.device-cart-l {
		height: 100rpx;
		/* 设置高度为100像素 */
		width: 700rpx;
		/* 设置宽度为700像素 */
		border-radius: 40rpx;
		/* 设置边框圆角为40像素 */
		margin-top: 30rpx;
		/* 设置上外边距为30像素 */
		display: flex;
		/* 使用弹性盒子布局 */
		justify-content: space-around;
		/* 在弹性容器中均匀分布子元素，两端对齐 */
		align-items: center;
		/* 在弹性容器中垂直居中对齐子元素 */
		box-shadow: 0 0 15rpx #ccc;
		/* 设置盒子阴影，颜色为灰色 */
	}

	/* 设备名称样式 */
	.dev-name {
		font-size: 20rpx;
		/* 设置字体大小为20像素 */
		text-align: center;
		/* 文本居中对齐 */
		color: #6d6d6d;
		/* 字体颜色为灰色 */
	}

	/* 设备图标样式 */
	.dev-logo {
		width: 70rpx;
		/* 设置宽度为70像素 */
		height: 70rpx;
		/* 设置高度为70像素 */
		margin-top: 10rpx;
		/* 设置上外边距为10像素 */
	}

	/* 设备数据样式 */
	.dev-data {
		font-size: 35rpx;
		/* 设置字体大小为40像素 */
		color: #6d6d6d;
		/* 字体颜色为灰色 */
	}
	
	/* 滑动条样式 */
	.ctrl-slider {
		width: 500rpx;
		/* 设置宽度为500像素 */
	}
	
</style>