-- ECE532 Final Exam
-- Zackary McClamma
-- 11-DEC-2019

library ieee;
  use ieee.std_logic_1164.all;

entity final_exam is
  port
  (
		i_clk         : in  std_logic := 'X'; -- clk
		i_reset_n     : in  std_logic := 'X'; -- reset_n
      i_uart_rxd      : in  std_logic := 'X'; -- rxd
      o_uart_txd      : out std_logic;         -- txd
      b_i2c_scl       : inout std_logic;
      b_i2c_sda       : inout std_logic;
      green_led	      : out std_logic;
      red_led         : out std_logic;
		o_sdram_addr      : out   std_logic_vector(12 downto 0);                    -- addr
		o_sdram_ba        : out   std_logic_vector(1 downto 0);                     -- ba
		o_sdram_cas_n     : out   std_logic;                                        -- cas_n
		o_sdram_cke       : out   std_logic;                                        -- cke
		o_sdram_cs_n      : out   std_logic;                                        -- cs_n
		b_sdram_dq        : inout std_logic_vector(31 downto 0) := (others => 'X'); -- dq
		o_sdram_dqm       : out   std_logic_vector(3 downto 0);                     -- dqm
		o_sdram_ras_n     : out   std_logic;                                        -- ras_n
		o_sdram_we_n      : out   std_logic;
		o_sdram_clk       : out   std_logic;
		display      : out std_logic_vector(6 downto 0);
		button : in std_logic	 

  );
end final_exam;

architecture sch of final_exam is

  component final_cpu is
	port (
      clk_clk           : in  std_logic := 'X'; -- clk
      reset_reset_n     : in  std_logic := 'X'; -- reset_n
		uart_rxd	: in  std_logic := 'X';
		uart_txd    : out std_logic;
      i2c_sda_in      : in std_logic;
      i2c_scl_in      : in std_logic;
      i2c_sda_oe      : out std_logic;
      i2c_scl_oe      : out std_logic;
      green_led_export : out std_logic;
      red_led_export   : out std_logic;
		sdram_addr      : out   std_logic_vector(12 downto 0);
		sdram_ba        : out   std_logic_vector(1 downto 0);
		sdram_cas_n     : out   std_logic;
		sdram_cke       : out   std_logic;                                        
		sdram_cs_n      : out   std_logic;                                        
		sdram_dq        : inout std_logic_vector(31 downto 0) := (others => 'X'); 
		sdram_dqm       : out   std_logic_vector(3 downto 0);                     
		sdram_ras_n     : out   std_logic;                                        
		sdram_we_n      : out   std_logic;
		sdram_clk_clk   : out   std_logic;
		button_export : in std_logic;
		display_export      : out std_logic_vector(6 downto 0)        -- export

        );
  end component final_cpu;

  signal w_i2c_sda_in     : std_logic;
  signal w_i2c_scl_in     : std_logic;
  signal w_i2c_sda_oe     : std_logic;
  signal w_i2c_scl_oe     : std_logic;
  signal w_sdram_clk  : std_logic;

begin

    b_i2c_scl <= '0' when w_i2c_scl_oe = '1' else 'Z';
    b_i2c_sda <= '0' when w_i2c_sda_oe = '1' else 'Z';
    w_i2c_scl_in <= b_i2c_scl;
    w_i2c_sda_in <= b_i2c_sda;
	 o_sdram_clk <= w_sdram_clk;

    u0 : component final_cpu
        port map (
      clk_clk           => i_clk,           --        clk.clk
      reset_reset_n     => i_reset_n,     --      reset.reset_n
		uart_rxd    => i_uart_rxd,
		uart_txd   => o_uart_txd,
      green_led_export => green_led,
      red_led_export => red_led,
      i2c_sda_in => w_i2c_sda_in,
      i2c_scl_in => w_i2c_scl_in,
      i2c_sda_oe => w_i2c_sda_oe,
      i2c_scl_oe => w_i2c_scl_oe,
		sdram_addr      => o_sdram_addr,
		sdram_ba        => o_sdram_ba,
		sdram_cas_n     => o_sdram_cas_n,
		sdram_cke       => o_sdram_cke,
		sdram_cs_n      => o_sdram_cs_n,
		sdram_dq        => b_sdram_dq,
		sdram_dqm       => o_sdram_dqm,
		sdram_ras_n     => o_sdram_ras_n,
		sdram_we_n      => o_sdram_we_n,
		sdram_clk_clk   => w_sdram_clk,
		button_export	=> button,
		display_export => display

        );
end sch;
