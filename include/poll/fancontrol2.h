////////////////////////// DYNAMIC FAN CONTROL #2 (SMART CURVE) ////////////////////////
		if(webman_config->fanc == FAN_AUTO2)
		{
			static u16 up_timer = 0;
			static u16 down_timer = 0;
			u8 target_pct = 25;

			// ۱. محاسبه دور فن هدف بر اساس منحنی پیوسته
			if(t1 <= 58)
			{
				target_pct = 25; // XMB / منوی اصلی / لود سبک
			}
			else if(t1 <= 64)
			{
				// بازه ۵۹ تا ۶۴ درجه: شیب ۲۶٪ تا ۳۲٪
				target_pct = 26 + ((t1 - 59) * (32 - 26)) / (64 - 59);
			}
			else if(t1 <= 69)
			{
				// بازه ۶۵ تا ۶۹ درجه: شیب ۳۳٪ تا ۳۹٪
				target_pct = 33 + ((t1 - 65) * (39 - 33)) / (69 - 65);
			}
			else if(t1 <= 74)
			{
				// بازه ۷۰ تا ۷۴ درجه (بازی‌های سنگین): شیب ۴۰٪ تا ۴۶٪
				target_pct = 40 + ((t1 - 70) * (46 - 40)) / (74 - 70);
			}
			else
			{
				// محدوده دمای بحرانی بالای ۷۵ درجه
				target_pct = 50 + ((t1 - 75) * 5);
				if(target_pct > 100) target_pct = 100;
			}

			// ۲. تبدیل درصد به مقدار استاندارد ۸ بیتی وب‌من (۰ تا ۲۵۵)
			u8 target_speed_8bit = (target_pct * 255) / 100;

			if(old_fan == 0) old_fan = target_speed_8bit;

			// ۳. فیلتر میرایی نامتقارن (Slew-rate Damping)
			if(old_fan < target_speed_8bit)
			{
				// افزایش نرم: هر ۲ چرخه ۱ واحد افزایش
				down_timer = 0;
				if(++up_timer >= 2)
				{
					up_timer = 0;
					old_fan++;
					set_fan_speed(old_fan);
				}
			}
			else if(old_fan > target_speed_8bit)
			{
				// کاهش بسیار کند و پسماند صوتی: هر ۸ چرخه ۱ واحد کاهش
				up_timer = 0;
				if(++down_timer >= 8)
				{
					down_timer = 0;
					old_fan--;
					set_fan_speed(old_fan);
				}
			}
		}
