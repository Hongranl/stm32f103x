问题：SPI2使用错误是因为和TIM1冲突，
现象：只能在初始化后500MS内使用
解决办法：屏蔽TIM1时钟总线，SPI2可以正常使用。（RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,DISABLE); 必须使用前者，后者不行，TIM_Cmd(TIM1,DISABLE);//失能TIM1）

