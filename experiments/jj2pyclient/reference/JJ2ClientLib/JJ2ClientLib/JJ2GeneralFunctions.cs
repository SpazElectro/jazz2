
namespace JJ2ClientLib
{
    public class JJ2GeneralFunctions
    {
        public static string GetGameModeShortString(byte gameMode, byte customMode)
        {
            if (customMode == 0)
            {
                switch (gameMode)
                {
                    case 0:
                        {
                            return "SP";
                        }

                    case 1:
                        {
                            return "Coop";
                        }

                    case 2:
                        {
                            return "Battle";
                        }

                    case 3:
                        {
                            return "Race";
                        }

                    case 4:
                        {
                            return "Treasure";
                        }

                    case 5:
                        {
                            return "CTF";
                        }

                    default:
                        {
                            return "";
                        }
                }
            }
            else
            {
                switch (customMode)
                {
                    case 1:
                        {
                            return "RT";
                        }

                    case 2:
                        {
                            return "LRS";
                        }

                    case 3:
                        {
                            return "XLRS";
                        }

                    case 4:
                        {
                            return "PEST";
                        }

                    case 11:
                        {
                            return "TB";
                        }

                    case 12:
                        {
                            return "JB";
                        }

                    case 13:
                        {
                            return "DCTF";
                        }

                    case 14:
                        {
                            return "FR";
                        }

                    case 15:
                        {
                            return "TLRS";
                        }

                    case 16:
                        {
                            return "Dom";
                        }

                    case 17:
                        {
                            return "HH";
                        }

                    default:
                        {
                            return "";
                        }
                }
            }
        }
    }
}