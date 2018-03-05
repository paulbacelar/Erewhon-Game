using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;

namespace Erewhon_Installer
{
    static class Program
    {
        /// <summary>
        /// Point d'entrée principal de l'application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //Debug.Assert(false);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
            var psi = new ProcessStartInfo(Path.Combine(Directory.GetCurrentDirectory(), "Utopia/ErewhonClient.exe"));
            psi.WorkingDirectory = Path.Combine(Directory.GetCurrentDirectory(), "Utopia");
            Process.Start(psi);
        }
    }
}
