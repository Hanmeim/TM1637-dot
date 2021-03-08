enum pins{
//%
}
}
//% color="#A5001D" iconWidth=50 iconHeight=40
namespace c_TM1637  {
    //% block="TM1637数码管初始化 CLK引脚：[CLK] DIO引脚：[DIO]" blocktype="command"
    //% CLK.shadow="dropdown" CLK.options="PIN_DigitalWrite"
    //% DIO.shadow="dropdown" DIO.options="PIN_DigitalWrite"
     export function TM1637_init(parameter: any, block: any) {
        let pin_clk = parameter.CLK.code
        let pin_dio = parameter.DIO.code
        Generator.addInclude("TM1637_include", "#include <TM1637.h>");
        Generator.addObject("my_TM1637",`TM1637`, `my_Tm1637(${pin_clk},${pin_dio});`);
        Generator.addSetup("TM1637_init", `my_Tm1637.init();`)
    }

    //% block="设置TM1637数码管亮度为[BRI]" blockType="command"
    //% BRI.shadow="range" BRI.params.min=0 BRI.params.max=7 BRI.defl=2
    export function TM1637_brightness(parameter: any, block: any) {
        let bright = parameter.BRI.code
        Generator.addCode(`my_Tm1637.set(${bright});`);
    }

    //% block="设置TM1637数码管显示数字：[VAL]" blockType="command"
    //% VAL.shadow="number" VAL.defl="12.34"

    export function TM1637_display(parameter: any, block: any) {
        let value = parameter.VAL.code
        Generator.addCode(`my_Tm1637.display(${value});`);
    }

    //% block="设置TM1637数码管第 [POS] 位显示 [NUM]" blockType="command"
    //% POS.shadow="range" POS.params.min=1 POS.params.max=4 POS.defl=1
    //% NUM.shadow="dropdownRound" NUM.options="NUM_DispData" Num.defl="0"
    export function TM1636_sel_disp(parameter: any, block: any) {
        let position = parameter.POS.code - 1;
        let disp_number = parameter.NUM.code;
        Generator.addCode(`my_Tm1637.display(${position},${disp_number});`);
    }

    //% block="TM1637数码管清屏" blockType="command"
    export function TM1636_clear(parameter: any, block: any) {
        Generator.addCode(`my_Tm1637.clearDisplay();`);
    }

    //% block="TM1637数码管显示时间[HH] : [MM]" blockType="command"
    //% HH.shadow="range" HH.params.min=0 HH.params.max=23 HH.defl=12
    //% MM.shadow="range" MM.params.min=0 MM.params.max=59 MM.defl=30
    export function TM1637_time(parameter: any, block: any){
        let hour = parameter.HH.code;
        let minute = parameter.MM.code;
        Generator.addCode(`my_Tm1637.point(1);`);
        Generator.addCode(`my_Tm1637.display(${hour*100+minute});`);
        Generator.addCode(`my_Tm1637.point(0);`);
    }
}
