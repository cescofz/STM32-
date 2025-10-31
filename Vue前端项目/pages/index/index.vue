<template>
	<div class="page">
		<div class="Author">
			<h1>作者:石家庄市电子信息学校 24-13班李佳澍</h1>
		</div>
		<div class="temp">
			<p>当前温度: {{ temperature || '加载中...' }}℃</p>
		</div>
		<div class="hum">
			<p>当前湿度: {{ humidity || '加载中...' }}%</p>
		</div>
		<div class="smoke">
			<p>烟雾:{{ smoke === 1 ? '有烟' : smoke === 0 ? '无烟' : '加载中...' }}</p>
		</div>

	</div>
</template>

<script>
	export default {
		data() {
			return {
				accessToken: '',
				temperature: null, // 新增温度变量
				humidity: null ,// 新增湿度变量
				smoke:null,
			}
		},
		onLoad() {
			// 修正：补充 then 回调的闭合和错误处理
			this.getToken()
				.then(token => {
					this.accessToken = token.AccessToken
					console.log('获取Token成功:', this.accessToken)
					// 获取Token成功后启动定时请求
					this.startDataPolling()
				})
				.catch(err => {
					console.error('获取Token失败:', err)
					this.temperature = '认证失败'
					this.humidity = '认证失败'
				})
		},
		onUnload() {
			if (this.pollingTimer) {
				clearInterval(this.pollingTimer)
			}
		},
		methods: {
			getToken() {
				return new Promise((resolve, reject) => {
					const dataInfo = {
						Account: "15511896620",
						Password: "123456"
					}
					uni.request({
						url: "http://api.nlecloud.com/Users/Login",
						method: 'POST',
						data: dataInfo,
						success: (response) => {
							if (response.data && response.data.ResultObj && response.data.ResultObj
								.AccessToken) {
								resolve({
									AccessToken: response.data.ResultObj.AccessToken
								})
							} else {
								reject(new Error('获取Token失败：响应格式不正确'))
							}
						},
						fail: (err) => {
							reject(err)
						}
					})
				})
			},
			fuc_req() {
				uni.request({
					url: "http://api.nlecloud.com/devices/1331745/sensors/temperature",
					method: "GET",
					header: {
						'AccessToken': this.accessToken
					},
					success: (res) => {
						console.log(res.data)
						this.temperature = res.data["ResultObj"]["Value"]
					},
				})
				uni.request({
					url: "http://api.nlecloud.com/devices/1331745/sensors/humidity",
					method: "GET",
					header: {
						'AccessToken': this.accessToken
					},
					success: (res) => {
						console.log(res.data)
						this.humidity = res.data["ResultObj"]["Value"]
					},
				})
				uni.request({
					url: "http://api.nlecloud.com/devices/1331745/sensors/smoke",
					method: "GET",
					header: {
						'AccessToken': this.accessToken
					},
					success: (res) => {
						console.log(res.data)
						this.smoke = res.data["ResultObj"]["Value"]
					},
				})	
			},
			startDataPolling() {
				// 立即执行一次，避免首次等待2秒
				this.fuc_req()
				// 每2秒执行一次，将定时器ID保存到实例中
				this.pollingTimer = setInterval(() => {
					this.fuc_req()
				}, 2000)
			}
		},
	}
</script>

<style scoped>
	.page {
		position: relative;
		width: 100vw;
		height: 100vh;
		overflow: hidden;
	}
	.page::before {
		content: '';
		position: absolute;
		top: -10px;
		left: -10px;
		right: -10px;
		bottom: -10px;
		background: url('/resource/image/background.webp') center/cover;
		filter: blur(8px);
		z-index: -1;
	}
	.smoke,
	.temp,
	.hum {
		padding: 2rem;
		color: red;
		font-size: 16px;
		text-shadow: 0 2px 4px rgba(0, 0, 0, 0.5);
		display: flex;
		justify-content: center;
	}
	.Author{
		font-size: 7px;
		color: #00FFFF;
	}
</style>